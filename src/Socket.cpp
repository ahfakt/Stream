#include "Stream/Socket.hpp"
#include <cstring>
#include <netdb.h>
#include <netinet/tcp.h>
#include <unistd.h>


namespace Stream {

Socket::Socket(int descriptor) noexcept
		: Input{false}
		, Output{false}
		, mDescriptor{descriptor}
{}

/**
 * @see	<a href="https://man7.org/linux/man-pages/man2/socket.2.html">socket()</a>
 * @details	Creates a socket resource.
 *			If <b>socket()</b> system call fails, it throws a Socket::Exception.
 */
Socket::Socket()
		: Socket{::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)}
{
	if (mDescriptor == -1)
		throw Exception{std::make_error_code(static_cast<std::errc>(errno))};
}

/**
 * @see	<a href="https://man7.org/linux/man-pages/man2/socket.2.html">socket()</a>
 * @see	<a href="https://man7.org/linux/man-pages/man2/bind.2.html">bind()</a>
 * @see	<a href="https://man7.org/linux/man-pages/man2/listen.2.html">listen()</a>
 */
Socket::Socket(Address const& address, int backlog)
		: Socket{}
{
	if (::bind(mDescriptor, &address, sizeof(struct sockaddr_in)) == -1 ||
		::listen(mDescriptor, backlog) == -1) {
		auto const errc{static_cast<std::errc>(errno)};
		if (::close(mDescriptor) == -1)
			LOG_ERR(::strerror(errno));
		throw Exception{std::make_error_code(errc)};
	}
}

/**
 * @see	<a href="https://man7.org/linux/man-pages/man2/socket.2.html">socket()</a>
 * @see	<a href="https://man7.org/linux/man-pages/man2/connect.2.html">connect()</a>
 */
Socket::Socket(Address const& address)
		: Socket{}
{
	if (::connect(mDescriptor, &address, sizeof(struct sockaddr_in)) == -1) {
		auto const errc{static_cast<std::errc>(errno)};
		if (::close(mDescriptor) == -1)
			LOG_ERR(::strerror(errno));
		throw Exception{std::make_error_code(errc)};
	}
}

Socket::Socket(Socket&& other) noexcept
		: Input{false}
		, Output{false}
		, mDescriptor{-1}
{ swap(*this, other); }

void
swap(Socket& a, Socket& b) noexcept
{ std::swap(a.mDescriptor, b.mDescriptor); }

Socket&
Socket::operator=(Socket other) noexcept
{
	swap(*this, other);
	return *this;
}

/**
 * @see		<a href="https://man7.org/linux/man-pages/man2/close.2.html">close()</a>
 * @see		<a href="https://man7.org/linux/man-pages/man3/sys_nerr.3.html">perror()</a>
 * @see		<a href="https://en.cppreference.com/w/cpp/io/c/std_streams">stdin, stdout, stderr</a>
 * @details	Closes the socket resource.
 *			If the <b>close()</b> system call fails, it writes the description of the error to the <b>standard error stream</b>.
 */
Socket::~Socket()
{
	if (mDescriptor != -1 && ::close(mDescriptor) == -1)
		LOG_ERR(::strerror(errno));
}

std::size_t
Socket::readBytes(std::byte* dest, std::size_t size)
{
	while (true) {
		auto r{::recv(mDescriptor, dest, size, 0)};
		if (r > 0)
			return r;
		if (r == 0)
			throw Input::Exception{std::make_error_code(std::errc::no_message_available)};
		if (errno != EINTR)
			throw Input::Exception{std::make_error_code(static_cast<std::errc>(errno))};
	}
}

std::size_t
Socket::writeBytes(std::byte const* src, std::size_t size)
{
	while (true) {
		if (auto r{::send(mDescriptor, src, size, MSG_NOSIGNAL)}; r >= 0)
			return r;
		if (errno != EINTR)
			throw Output::Exception{std::make_error_code(static_cast<std::errc>(errno))};
	}
}

/**
 * @see	<a href="https://man7.org/linux/man-pages/man2/bind.2.html">bind()</a>
 */
std::error_code
Socket::bind(Address const& address) noexcept
{
	return ::bind(mDescriptor, &address, sizeof(struct sockaddr_in)) == 0
		? std::error_code{}
		: std::make_error_code(static_cast<std::errc>(errno));
}

/**
 * @see	<a href="https://man7.org/linux/man-pages/man2/listen.2.html">listen()</a>
 */
std::error_code
Socket::listen(int backlog) noexcept
{
	return ::listen(mDescriptor, backlog) == 0
		? std::error_code{}
		: std::make_error_code(static_cast<std::errc>(errno));
}

/**
 * @see	<a href="https://man7.org/linux/man-pages/man2/accept.2.html">accept()</a>
 */
std::expected<Socket, std::error_code>
Socket::accept() const noexcept
{
	if (auto r{::accept(mDescriptor, nullptr, nullptr)}; r != -1)
		return Socket{r};
	return std::unexpected{std::make_error_code(static_cast<std::errc>(errno))};
}

/**
 * @see	<a href="https://man7.org/linux/man-pages/man2/connect.2.html">connect()</a>
 */
std::error_code
Socket::connect(Address const& address) noexcept
{
	return ::connect(mDescriptor, &address, sizeof(struct sockaddr_in)) == 0
		? std::error_code{}
		: std::make_error_code(static_cast<std::errc>(errno));
}

/**
 * @see	<a href="https://man7.org/linux/man-pages/man2/getsockopt.2.html">getsockopt()</a>
 * @see	<a href="https://linux.die.net/man/7/tcp#:~:text=TCP_MAXSEG">TCP_MAXSEG</a>
 */
std::expected<int, std::error_code>
Socket::getMSS() const noexcept
{
	int mss;
	socklen_t optlen{sizeof mss};
	if (::getsockopt(mDescriptor, IPPROTO_TCP, TCP_MAXSEG, &mss, &optlen) != -1)
		return mss;
	return std::unexpected{std::make_error_code(static_cast<std::errc>(errno))};
}

std::expected<::timeval, std::error_code>
Socket::getRecvTimeout() const noexcept
{
	timeval recvTimeout;
	socklen_t optlen{sizeof recvTimeout};
	if (::getsockopt(mDescriptor, SOL_SOCKET, SO_RCVTIMEO, &recvTimeout, &optlen) != -1)
		return recvTimeout;
	return std::unexpected{std::make_error_code(static_cast<std::errc>(errno))};
}

std::error_code
Socket::setRecvTimeout(::timeval timeout) noexcept
{
	return ::setsockopt(mDescriptor, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof timeout) == 0
		? std::error_code{}
		: std::make_error_code(static_cast<std::errc>(errno));
}

std::expected<::timeval, std::error_code>
Socket::getSendTimeout() const noexcept
{
	timeval sendTimeout;
	socklen_t optlen{sizeof sendTimeout};
	if (::getsockopt(mDescriptor, SOL_SOCKET, SO_SNDTIMEO, &sendTimeout, &optlen) != -1)
		return sendTimeout;
	return std::unexpected{std::make_error_code(static_cast<std::errc>(errno))};
}

std::error_code
Socket::setSendTimeout(::timeval timeout) noexcept
{
	return ::setsockopt(mDescriptor, SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof timeout) == 0
		? std::error_code{}
		: std::make_error_code(static_cast<std::errc>(errno));
}

/**
 * @see	<a href="https://man7.org/linux/man-pages/man7/ip.7.html">struct sockaddr_in</a>
 * @see	<a href="https://man7.org/linux/man-pages/man3/inet_aton.3.html">inet_aton()</a>
 */
Socket::Address::Inet::Inet(char const* host, std::uint16_t port)
{
	struct addrinfo hints{};
	hints.ai_flags = 0;
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	struct addrinfo* info{nullptr};
	if (int e = ::getaddrinfo(host, nullptr, &hints, &info))
		throw Socket::Address::Exception{static_cast<Socket::Address::Exception::Code>(e), host};

	auto* This{reinterpret_cast<sockaddr_in*>(this)};
	This->sin_family = AF_INET;
	This->sin_port = ::htons(port);
	This->sin_addr = reinterpret_cast<sockaddr_in*>(info->ai_addr)->sin_addr;
	::freeaddrinfo(info);
}

sockaddr_in*
Socket::Address::Inet::operator->() noexcept
{ return reinterpret_cast<sockaddr_in*>(this); }

sockaddr_in const*
Socket::Address::Inet::operator->() const noexcept
{ return reinterpret_cast<sockaddr_in const*>(this); }

std::error_code
make_error_code(Socket::Address::Exception::Code e) noexcept
{
	static struct : std::error_category {

		char const*
		name() const noexcept override
		{ return "Stream::Socket::Address"; }

		std::string
		message(int e) const noexcept override
		{ return ::gai_strerror(e); }

	} const cat;

	return {static_cast<int>(e), cat};
}

}//namespace Stream

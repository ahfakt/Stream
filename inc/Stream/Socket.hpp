#pragma once

#include "InOut.hpp"
#include <expected>
#include <netinet/in.h>
#include <sys/socket.h>


namespace Stream {

/**
 * %Socket resource
 * @class	Socket Socket.hpp "Stream/Socket.hpp"
 */
class Socket : public Input, public Output {

	int mDescriptor;

	explicit
	Socket(int descriptor) noexcept;

	std::size_t
	readBytes(std::byte* dest, std::size_t size) final;

	std::size_t
	writeBytes(std::byte const* src, std::size_t size) final;

public:

	struct Exception : std::system_error
	{ using std::system_error::system_error; };

	struct Address : sockaddr {
		struct Exception : std::system_error {
			using std::system_error::system_error;
			enum class Code : int {};
		};//struct Stream::Socket::Address::Exception

		struct Inet;
	};//struct Stream::Socket::Address

	/**
	 * Construct a IPv4 TCP Socket resource.
	 * @throws	Socket::Exception
	 */
	Socket();

	/**
	 * Construct a TCP server Socket.
	 * @param[in]	address Address to be bound to accept incoming connections
	 * @param[in]	backlog Maximum length of pending connections queue
	 * @throws	Socket::Exception
	 */
	Socket(Address const& address, int backlog);

	/**
	 * Construct a TCP client Socket.
	 * @param[in]	address Address to connect
	 * @throws	Socket::Exception
	 */
	explicit
	Socket(Address const& address);

	Socket(Socket const&) = delete;

	Socket(Socket&& other) noexcept;

	friend void
	swap(Socket& a, Socket& b) noexcept;

	Socket&
	operator=(Socket other) noexcept;

	~Socket();

	/**
	 * Bind this Socket to an address.
	 * @param[in]	address Address to be bound
	 */
	[[nodiscard]]
	std::error_code
	bind(Address const& address) noexcept;

	/**
	 * Put this Socket into the listening state.
	 * @param[in]	backlog Maximum length of pending connections queue
	 */
	[[nodiscard]]
	std::error_code
	listen(int backlog) noexcept;

	/**
	 * Accept a new client connection.
	 * @returns	Socket
	 */
	[[nodiscard]]
	std::expected<Socket, std::error_code>
	accept() const noexcept;

	/**
	 * Connect to a server at the address.
	 * @param[in]	address Address to connect
	 */
	[[nodiscard]]
	std::error_code
	connect(Address const& address) noexcept;

	/**
	 * Get the maximum segment size of the TCP connection.
	 * @return	Maximum segment size in bytes
	 */
	[[nodiscard]]
	std::expected<int, std::error_code>
	getMSS() const noexcept;

	[[nodiscard]]
	std::expected<::timeval, std::error_code>
	getRecvTimeout() const noexcept;

	[[nodiscard]]
	std::error_code
	setRecvTimeout(::timeval timeout) noexcept;

	[[nodiscard]]
	std::expected<::timeval, std::error_code>
	getSendTimeout() const noexcept;

	[[nodiscard]]
	std::error_code
	setSendTimeout(::timeval timeout) noexcept;

};//class Stream::Socket

struct Socket::Address::Inet : Socket::Address {

	/**
	 * @throws Socket::Adress::Exception
	 */
	Inet(char const* host, std::uint16_t port);

	sockaddr_in*
	operator->() noexcept;

	sockaddr_in const*
	operator->() const noexcept;

};//struct Stream::Socket::Adress::Inet

std::error_code
make_error_code(Socket::Address::Exception::Code e) noexcept;

}//namespace Stream

namespace std {

template <>
struct is_error_code_enum<Stream::Socket::Address::Exception::Code> : true_type {};

}//namespace std

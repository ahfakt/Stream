#ifndef STREAM_SOCKET_HPP
#define STREAM_SOCKET_HPP

#include "InOut.hpp"
#include <netinet/in.h>
#include <sys/socket.h>

namespace Stream {

/**
 * @brief	Socket resource
 * @class	Socket Socket.hpp "Stream/Socket.hpp"
 */
class Socket : public Input, public Output {
	int mDescriptor{-1};

	explicit Socket(int descriptor);

	std::size_t
	readBytes(std::byte* dest, std::size_t size) override;

	std::size_t
	writeBytes(std::byte const* src, std::size_t size) override;

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
	 * @brief	Construct a IPv4 TCP Socket resource.
	 * @throws	Socket::Exception
	 */
	Socket();

	/**
	 * @brief	Construct a TCP server Socket.
	 * @param[in]	address Address to be bound to accept incoming connections
	 * @param[in]	backlog Maximum length of pending connections queue
	 * @throws	Socket::Exception
	 */
	Socket(Address const& address, int backlog);

	/**
	 * @brief	Construct a TCP client Socket.
	 * @param[in]	address Address to connect
	 * @throws	Socket::Exception
	 */
	explicit Socket(Address const& address);

	Socket(Socket const&) = delete;

	Socket(Socket&& other) noexcept;

	friend void
	swap(Socket& a, Socket& b) noexcept;

	Socket&
	operator=(Socket&& other) noexcept;

	~Socket();

	/**
	 * @brief	Bind this Socket to an address.
	 * @param[in]	address Address to be bound
	 * @throws	Socket::Exception
	 */
	void
	bind(Address const& address);

	/**
	 * @brief	Put this Socket into the listening state.
	 * @param[in]	backlog Maximum length of pending connections queue
	 * @throws	Socket::Exception
	 */
	void
	listen(int backlog);

	/**
	 * @brief	Accept a new client connection.
	 * @returns	Socket
	 * @throws	Socket::Exception
	 */
	[[nodiscard]] Socket
	accept() const;

	/**
	 * @brief	Connect to a server at the address.
	 * @param[in]	address Address to connect
	 * @throws	Socket::Exception
	 */
	void
	connect(Address const& address);

	/**
	 * @brief	Get the maximum segment size of the TCP connection.
	 * @return	Maximum segment size in bytes
	 * @throws	Socket::Exception
	 */
	[[nodiscard]] int
	getMSS() const;

	[[nodiscard]] timeval
	getRecvTimeout() const;

	void
	setRecvTimeout(timeval timeout);

	[[nodiscard]] timeval
	getSendTimeout() const;

	void
	setSendTimeout(timeval timeout);
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

#endif //STREAM_SOCKET_HPP

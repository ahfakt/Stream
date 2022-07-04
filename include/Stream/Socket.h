#ifndef STREAM_SOCKET_H
#define STREAM_SOCKET_H

#include "InOut.h"
#include "IO/Socket.h"

namespace Stream {

/**
 * @brief	IO::Socket Input stream
 * @class	SocketInput Socket.h "Stream/Socket.h"
 */
class SocketInput : public Input {
	IO::Socket* mSocket;

	std::size_t
	readBytes(std::byte* dest, std::size_t size) override;

public:
	struct Exception : Input::Exception
	{ using Input::Exception::Exception; };

	SocketInput() noexcept;

	SocketInput(SocketInput const&) = delete;

	SocketInput(SocketInput&& other) noexcept;

	friend void
	swap(SocketInput& a, SocketInput& b) noexcept;

	SocketInput&
	operator=(SocketInput&& other) noexcept;

	friend SocketInput&
	operator>>(IO::Socket& socket, SocketInput& socketInput) noexcept;

	friend SocketInput&
	operator>>(std::nullptr_t, SocketInput& socketInput) noexcept;
};//class SocketInput

/**
 * @brief IO::Socket Output stream
 * @class SocketOutput Socket.h "Stream/Socket.h"
 */
class SocketOutput : public Output {
	IO::Socket* mSocket;

	std::size_t
	writeBytes(std::byte const* src, std::size_t size) override;

public:
	struct Exception : Output::Exception
	{ using Output::Exception::Exception; };

	SocketOutput() noexcept;

	SocketOutput(SocketOutput const&) = delete;

	SocketOutput(SocketOutput&& other) noexcept;

	friend void
	swap(SocketOutput& a, SocketOutput& b) noexcept;

	SocketOutput&
	operator=(SocketOutput&& other) noexcept;

	friend SocketOutput&
	operator<<(IO::Socket& socket, SocketOutput& socketOutput) noexcept;

	friend SocketOutput&
	operator<<(std::nullptr_t, SocketOutput& socketOutput) noexcept;
};//class SocketOutput

/**
 * @brief IO::Socket Input and Output stream
 * @class Socket Socket.h "Stream/Socket.h"
 */
class Socket : public SocketInput, public SocketOutput {};

void
swap(Socket& a, Socket& b) noexcept;

template <typename T>
concept SocketIn = std::is_base_of_v<SocketInput, T>;

template <typename T>
concept SocketOut = std::is_base_of_v<SocketOutput, T>;

template <typename T>
concept SocketInOut = SocketIn<T> && SocketOut<T>;

auto&
operator<=>(IO::Socket& socket, SocketInOut auto& socketInOut) noexcept
{
	socket >> socketInOut;
	socket << socketInOut;
	return socketInOut;
}

auto&
operator<=>(std::nullptr_t, SocketInOut auto& socketInOut) noexcept
{
	nullptr >> socketInOut;
	nullptr << socketInOut;
	return socketInOut;
}

}//namespace Stream

#endif //STREAM_SOCKET_H

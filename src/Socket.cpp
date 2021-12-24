#include "Stream/Socket.h"
#include <netinet/in.h>

namespace Stream {

SocketInput::SocketInput() noexcept
{ nullptr >> *this; }

SocketInput::SocketInput(SocketInput&& other) noexcept
{ swap(*this, other); }

void
swap(SocketInput& a, SocketInput& b) noexcept
{ std::swap(a.mSocket, b.mSocket); }

SocketInput&
SocketInput::operator=(SocketInput&& other) noexcept
{
	swap(*this, other);
	return *this;
}

std::size_t
SocketInput::readBytes(std::byte* dest, std::size_t size)
{
	try_again:
	ssize_t res = ::recv(mSocket->mDescriptor, dest, size, 0);
	if (res <= 0) {
		if (res < 0) {
			if (errno == EINTR)
				goto try_again;
			else
				throw Exception(std::make_error_code(static_cast<std::errc>(errno)));
		}
		throw Exception(std::make_error_code(static_cast<std::errc>(ENODATA)));
	}
	return res;
}

SocketInput&
operator>>(IO::Socket& socket, SocketInput& socketInput) noexcept
{
	socketInput.mSocket = &socket;
	return socketInput;
}

SocketInput&
operator>>(std::nullptr_t, SocketInput& socketInput) noexcept
{
	static int instance = -1;
	socketInput.mSocket = reinterpret_cast<IO::Socket*>(&instance);
	return socketInput;
}

SocketOutput::SocketOutput() noexcept
{ nullptr << *this; }

SocketOutput::SocketOutput(SocketOutput&& other) noexcept
{ swap(*this, other); }

void
swap(SocketOutput& a, SocketOutput& b) noexcept
{ std::swap(a.mSocket, b.mSocket); }

SocketOutput&
SocketOutput::operator=(SocketOutput&& other) noexcept
{
	swap(*this, other);
	return *this;
}

/**
 * @todo		BSD SIGNOPIPE
 */
std::size_t
SocketOutput::writeBytes(std::byte const* src, std::size_t size)
{
	try_again:
	ssize_t res = ::send(mSocket->mDescriptor, src, size, MSG_NOSIGNAL);
	if (res < 0) {
		if (errno == EINTR)
			goto try_again;
		else
			throw Exception(std::make_error_code(static_cast<std::errc>(errno)));
	}
	return res;
}

SocketOutput&
operator<<(IO::Socket& socket, SocketOutput& socketOutput) noexcept
{
	socketOutput.mSocket = &socket;
	return socketOutput;
}

SocketOutput&
operator<<(std::nullptr_t, SocketOutput& socketOutput) noexcept
{
	static int instance = -1;
	socketOutput.mSocket = reinterpret_cast<IO::Socket*>(&instance);
	return socketOutput;
}

void
swap(Socket& a, Socket& b) noexcept
{
	swap(static_cast<SocketInput&>(a), static_cast<SocketInput&>(b));
	swap(static_cast<SocketOutput&>(a), static_cast<SocketOutput&>(b));
}

}//namespace Stream

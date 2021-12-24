#include "Stream/Pipe.h"
#include <unistd.h>

namespace Stream {

PipeInput::PipeInput() noexcept
{ nullptr >> *this; }

PipeInput::PipeInput(PipeInput&& other) noexcept
{ swap(*this, other); }

void
swap(PipeInput& a, PipeInput& b) noexcept
{ std::swap(a.mPipe, b.mPipe); }

PipeInput&
PipeInput::operator=(PipeInput&& other) noexcept
{
	swap(*this, other);
	return *this;
}

std::size_t
PipeInput::readBytes(std::byte* dest, std::size_t size)
{
	try_again:
	ssize_t res = ::read(mPipe->mReadDescriptor, dest, size);
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

PipeInput&
operator>>(IO::Pipe& pipe, PipeInput& pipeInput) noexcept
{
	pipeInput.mPipe = &pipe;
	return pipeInput;
}

PipeInput&
operator>>(std::nullptr_t, PipeInput& pipeInput) noexcept
{
	static int instance[2] {-1, -1};
	pipeInput.mPipe = reinterpret_cast<IO::Pipe*>(instance);
	return pipeInput;
}

PipeOutput::PipeOutput() noexcept
{ nullptr << *this; }

PipeOutput::PipeOutput(PipeOutput&& other) noexcept
{ swap(*this, other); }

void
swap(PipeOutput& a, PipeOutput& b) noexcept
{ std::swap(a.mPipe, b.mPipe); }

PipeOutput&
PipeOutput::operator=(PipeOutput&& other) noexcept
{
	swap(*this, other);
	return *this;
}

std::size_t
PipeOutput::writeBytes(std::byte const* src, std::size_t size)
{
	try_again:
	ssize_t res = ::write(mPipe->mWriteDescriptor, src, size);
	if (res < 0) {
		if (errno == EINTR)
			goto try_again;
		else
			throw Exception(std::make_error_code(static_cast<std::errc>(errno)));
	}
	return res;
}

PipeOutput&
operator<<(IO::Pipe& pipe, PipeOutput& pipeOutput) noexcept
{
	pipeOutput.mPipe = &pipe;
	return pipeOutput;
}

PipeOutput&
operator<<(std::nullptr_t, PipeOutput& pipeOutput) noexcept
{
	static int instance[2] {-1, -1};
	pipeOutput.mPipe = reinterpret_cast<IO::Pipe*>(instance);
	return pipeOutput;
}

void
swap(Pipe& a, Pipe& b) noexcept
{
	swap(static_cast<PipeInput&>(a), static_cast<PipeInput&>(b));
	swap(static_cast<PipeOutput&>(a), static_cast<PipeOutput&>(b));
}

}//namespace Stream

#include "Stream/Pipe.hpp"
#include <fcntl.h>
#include <unistd.h>

#define ExpectNNeg(x) if (0 > x) throw Exception{std::make_error_code(static_cast<std::errc>(errno))}
#define ExpectNNegP(x, p) if (0 > x) throw Exception{std::make_error_code(static_cast<std::errc>(errno)), p}

namespace Stream {

/**
 * @see		<a href="https://man7.org/linux/man-pages/man2/pipe.2.html">pipe()</a>
 * @details	Creates a pipe resource.
 *			If <b>pipe()</b> system call fails, it throws a Pipe::Exception.
 */
Pipe::Pipe()
{ ExpectNNeg(pipe(mDescriptors)); }

Pipe::Pipe(Pipe&& other) noexcept
{ swap(*this, other); }

void
swap(Pipe& a, Pipe& b) noexcept
{
	std::swap(a.mRDescriptor, b.mRDescriptor);
	std::swap(a.mWDescriptor, b.mWDescriptor);
}

Pipe&
Pipe::operator=(Pipe&& other) noexcept
{
	swap(*this, other);
	return *this;
}

/**
 * @see		<a href="https://man7.org/linux/man-pages/man2/close.2.html">close()</a>
 * @see		<a href="https://man7.org/linux/man-pages/man3/sys_nerr.3.html">perror()</a>
 * @see		<a href="https://en.cppreference.com/w/cpp/io/c/std_streams">stdin, stdout, stderr</a>
 * @details	Closes the pipe resource.
 *			If any of the <b>close()</b> system calls fails, it writes the descriptions of the errors to the <b>standard error stream</b>.
 */
Pipe::~Pipe()
{
	if (mRDescriptor > 2 && close(mRDescriptor) < 0)
		perror(nullptr);
	if (mWDescriptor > 2 && close(mWDescriptor) < 0)
		perror(nullptr);
}

std::size_t
Pipe::readBytes(std::byte* dest, std::size_t size)
{
	while (true) {
		auto r{::read(mRDescriptor, dest, size)};
		if (r > 0)
			return r;
		if (r == 0)
			throw Input::Exception{std::make_error_code(std::errc::no_message_available)};
		if (errno != EINTR)
			throw Input::Exception{std::make_error_code(static_cast<std::errc>(errno))};
	}
}

std::size_t
Pipe::writeBytes(std::byte const* src, std::size_t size)
{
	while (true) {
		auto r{::write(mWDescriptor, src, size)};
		if (r >= 0)
			return r;
		if (errno != EINTR)
			throw Output::Exception{std::make_error_code(static_cast<std::errc>(errno))};
	}
}

/**
 * @see		<a href="https://man7.org/linux/man-pages/man2/fcntl.2.html">fcntl()</a>
 * @see		<a href="https://man7.org/linux/man-pages/man2/fcntl.2.html#:~:text=F_SETPIPE_SZ">F_SETPIPE_SZ</a>
 * @details	Sets the buffer size of the pipe.
 *			If the <b>fcntl()</b> system call fails, it throws a Pipe::Exception. Otherwise it returns the actual buffer size that is set.
 */
std::size_t
Pipe::setBufferSize(std::size_t bufferSize)
{
	auto r{fcntl(mWDescriptor, F_SETPIPE_SZ, bufferSize)};
	ExpectNNegP(r, std::to_string(bufferSize));
	return r;
}

/**
 * @see		<a href="https://man7.org/linux/man-pages/man2/fcntl.2.html">fcntl()</a>
 * @see		<a href="https://man7.org/linux/man-pages/man2/fcntl.2.html#:~:text=F_GETPIPE_SZ">F_GETPIPE_SZ</a>
 * @details	Gets the buffer size of the pipe.
 *			If the <b>fcntl()</b> system call fails, it throws a Pipe::Exception. Otherwise it returns the buffer size.
 */
std::size_t
Pipe::getBufferSize() const
{
	auto r{fcntl(mWDescriptor, F_GETPIPE_SZ)};
	ExpectNNeg(r);
	return r;
}

}//namespace Stream

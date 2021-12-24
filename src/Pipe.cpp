#include "Stream/Pipe.hpp"
#include <cstring>
#include <fcntl.h>
#include <unistd.h>


namespace Stream {

/**
 * @see		<a href="https://man7.org/linux/man-pages/man2/pipe.2.html">pipe()</a>
 * @details	Creates a pipe resource.
 *			If <b>pipe()</b> system call fails, it throws a Pipe::Exception.
 */
Pipe::Pipe()
		: Input{false}
		, Output{false}
{
	if (::pipe(mDescriptors) == -1)
		throw Pipe::Exception{std::make_error_code(static_cast<std::errc>(errno))};
}

Pipe::Pipe(Pipe&& other) noexcept
		: Input{false}
		, Output{false}
		, mDescriptors{-1, -1}
{ swap(*this, other); }

void
swap(Pipe& a, Pipe& b) noexcept
{
	std::swap(a.mReadDescriptor, b.mReadDescriptor);
	std::swap(a.mWriteDescriptor, b.mWriteDescriptor);
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
	if (mReadDescriptor != -1 && ::close(mReadDescriptor) == -1)
		LOG_ERR(::strerror(errno));
	if (mWriteDescriptor != -1 && ::close(mWriteDescriptor) == -1)
		LOG_ERR(::strerror(errno));
}

std::size_t
Pipe::readBytes(std::byte* dest, std::size_t size)
{
	while (true) {
		auto r{::read(mReadDescriptor, dest, size)};
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
		if (auto r{::write(mWriteDescriptor, src, size)}; r >= 0)
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
std::expected<std::size_t, std::error_code>
Pipe::setBufferSize(std::size_t bufferSize) noexcept
{
	std::expected<std::size_t, std::error_code> r{::fcntl(mWriteDescriptor, F_SETPIPE_SZ, bufferSize)};
	if (r.value() != -1)
		return r;
	return std::unexpected{std::make_error_code(static_cast<std::errc>(errno))};
}

/**
 * @see		<a href="https://man7.org/linux/man-pages/man2/fcntl.2.html">fcntl()</a>
 * @see		<a href="https://man7.org/linux/man-pages/man2/fcntl.2.html#:~:text=F_GETPIPE_SZ">F_GETPIPE_SZ</a>
 * @details	Gets the buffer size of the pipe.
 *			If the <b>fcntl()</b> system call fails, it throws a Pipe::Exception. Otherwise it returns the buffer size.
 */
std::expected<std::size_t, std::error_code>
Pipe::getBufferSize() const noexcept
{
	std::expected<std::size_t, std::error_code> r{::fcntl(mWriteDescriptor, F_GETPIPE_SZ)};
	if (r.value() != -1)
		return r;
	return std::unexpected{std::make_error_code(static_cast<std::errc>(errno))};
}

}//namespace Stream

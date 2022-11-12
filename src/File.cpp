#include "Stream/File.hpp"
#include <sys/stat.h>
#include <unistd.h>

#define ExpectNNeg(x) if (0 > x) throw Exception{std::make_error_code(static_cast<std::errc>(errno))}
#define ExpectNNegP(x, p) if (0 > x) throw Exception{std::make_error_code(static_cast<std::errc>(errno)), p}

namespace Stream {

/**
 * @see		<a href="https://man7.org/linux/man-pages/man2/open.2.html">open()</a>
 * @details	Creates a file resource with given @p name and open @p mode.
 *			If <b>open()</b> system call fails, it throws a File::Exception.
 */
File::File(std::string const& name, Mode mode)
		: mDescriptor{open(name.c_str(), static_cast<int>(mode), S_IWUSR | S_IRUSR | S_IRGRP | S_IROTH)}
{ ExpectNNegP(mDescriptor, name); }

File::File(File&& other) noexcept
{ swap(*this, other); }

void
swap(File& a, File& b) noexcept
{ std::swap(a.mDescriptor, b.mDescriptor); }

File&
File::operator=(File&& other) noexcept
{
	swap(*this, other);
	return *this;
}

/**
 * @see		<a href="https://man7.org/linux/man-pages/man2/close.2.html">close()</a>
 * @see		<a href="https://man7.org/linux/man-pages/man3/sys_nerr.3.html">perror()</a>
 * @see		<a href="https://en.cppreference.com/w/cpp/io/c/std_streams">stdin, stdout, stderr</a>
 * @details	Closes the file resource.
 *			If the <b>close()</b> system call fails, it writes the description of the error to the <b>standard error stream</b>.
 */
File::~File()
{
	if (fsync(mDescriptor) < 0)
		perror(nullptr);
	if (mDescriptor > 2 && close(mDescriptor) < 0)
		perror(nullptr);
}

std::size_t
File::readBytes(std::byte* dest, std::size_t size)
{
	while (true) {
		auto r{::read(mDescriptor, dest, size)};
		if (r > 0)
			return r;
		if (r == 0)
			throw Input::Exception{std::make_error_code(std::errc::no_message_available)};
		if (errno != EINTR)
			throw Input::Exception{std::make_error_code(static_cast<std::errc>(errno))};
	}
}

std::size_t
File::writeBytes(std::byte const* src, std::size_t size)
{
	while (true) {
		auto r{::write(mDescriptor, src, size)};
		if (r >= 0)
			return r;
		if (errno != EINTR)
			throw Output::Exception{std::make_error_code(static_cast<std::errc>(errno))};
	}
}

/**
 * @see		<a href="https://man7.org/linux/man-pages/man2/fdatasync.2.html">fdatasync()</a>
 */
void
File::flush()
{ ExpectNNeg(fdatasync(mDescriptor)); }

/**
 * @see		<a href="https://man7.org/linux/man-pages/man2/lstat.2.html#:~:text=The%20fields%20in%20the%20stat,the%20file%20type%20and%20mode.">struct stat</a>
 * @see		<a href="https://man7.org/linux/man-pages/man3/fstat.3p.html">fstat()</a>
 * @details	Gets the block size information of the file from a <b>struct stat</b> filled by the <b>fstat()</b> function.
 *			If the <b>fstat()</b> system call fails, it throws a File::Exception. Otherwise it returns the block size.
 */
blksize_t
File::getBlockSize() const
{
	struct stat fileStatus;
	ExpectNNeg(fstat(mDescriptor, &fileStatus));
	return fileStatus.st_blksize;
}

off_t
File::getFileSize() const
{
	struct stat fileStatus;
	ExpectNNeg(fstat(mDescriptor, &fileStatus));
	return fileStatus.st_size;
}

}//namespace Stream

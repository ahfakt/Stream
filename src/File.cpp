#include "Stream/File.h"
#include <unistd.h>

namespace Stream {

FileInput::FileInput() noexcept
{ nullptr >> *this; }

FileInput::FileInput(FileInput&& other) noexcept
{ swap(*this, other); }

void
swap(FileInput& a, FileInput& b) noexcept
{
	std::swap(a.mFile, b.mFile);
	std::swap(a.mOffset, b.mOffset);
}

FileInput&
FileInput::operator=(FileInput&& other) noexcept
{
	swap(*this, other);
	return *this;
}

std::size_t
FileInput::readBytes(std::byte* dest, std::size_t size)
{
	try_again:
	ssize_t res = ::pread(mFile->mDescriptor, dest, size, mOffset);
	if (res <= 0) {
		if (res < 0) {
			if (errno == EINTR)
				goto try_again;
			else
				throw Exception(std::make_error_code(static_cast<std::errc>(errno)));
		}
		throw Exception(std::make_error_code(static_cast<std::errc>(ENODATA)));
	}
	mOffset += res;
	return res;
}

FileInput&
operator>>(IO::File& file, FileInput& fileInput) noexcept
{
	fileInput.mFile = &file;
	return fileInput;
}

FileInput&
operator>>(std::nullptr_t, FileInput& fileInput) noexcept
{
	static int instance = -1;
	fileInput.mFile = reinterpret_cast<IO::File*>(&instance);
	return fileInput;
}

FileInput&
FileInput::seekGet(off_t pos) noexcept
{
	mOffset = pos;
	return *this;
}

off_t
FileInput::tellGet() const noexcept
{ return mOffset; }

FileOutput::FileOutput() noexcept
{ nullptr << *this; }

FileOutput::FileOutput(FileOutput&& other) noexcept
{ swap(*this, other); }

void
swap(FileOutput& a, FileOutput& b) noexcept
{
	std::swap(a.mFile, b.mFile);
	std::swap(a.mOffset, b.mOffset);
}

FileOutput&
FileOutput::operator=(FileOutput&& other) noexcept
{
	swap(*this, other);
	return *this;
}

/**
 * @see		<a href="https://man7.org/linux/man-pages/man2/fdatasync.2.html">fsync()</a>
 */
FileOutput::~FileOutput()
{
	if (fsync(mFile->mDescriptor) < 0)
		perror(nullptr);
}

std::size_t
FileOutput::writeBytes(std::byte const* src, std::size_t size)
{
	try_again:
	ssize_t res = ::pwrite(mFile->mDescriptor, src, size, mOffset);
	if (res < 0) {
		if (errno == EINTR)
			goto try_again;
		else
			throw Exception(std::make_error_code(static_cast<std::errc>(errno)));
	}
	mOffset += res;
	return res;
}

/**
 * @see		<a href="https://man7.org/linux/man-pages/man2/fdatasync.2.html">fdatasync()</a>
 */
void
FileOutput::flush()
{
	if (fdatasync(mFile->mDescriptor) < 0)
		throw Exception(std::make_error_code(static_cast<std::errc>(errno)));
}

FileOutput&
operator<<(IO::File& file, FileOutput& fileOutput) noexcept
{
	fileOutput.mFile = &file;
	return fileOutput;
}

FileOutput&
operator<<(std::nullptr_t, FileOutput& fileOutput) noexcept
{
	static int instance = -1;
	fileOutput.mFile = reinterpret_cast<IO::File*>(&instance);
	return fileOutput;
}

FileOutput&
FileOutput::seekPut(off_t pos)
{
	mOffset = pos;
	return *this;
}

off_t
FileOutput::tellPut() const noexcept
{ return mOffset; }

void
swap(File& a, File& b) noexcept
{
	swap(static_cast<FileInput&>(a), static_cast<FileInput&>(b));
	swap(static_cast<FileOutput&>(a), static_cast<FileOutput&>(b));
}

}//namespace Stream

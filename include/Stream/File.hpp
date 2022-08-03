#ifndef STREAM_FILE_HPP
#define STREAM_FILE_HPP

#include "InOut.hpp"
#include <IO/File.hpp>

namespace Stream {

/**
 * @brief	IO::File Input stream
 * @class	FileInput File.hpp "Stream/File.hpp"
 */
class FileInput : public Input {
	IO::File* mFile;
	off_t mOffset = 0;

	std::size_t
	readBytes(std::byte* dest, std::size_t size) override;

public:
	struct Exception : Input::Exception
	{ using Input::Exception::Exception; };

	FileInput() noexcept;

	FileInput(FileInput const&) = delete;

	FileInput(FileInput&& other) noexcept;

	friend void
	swap(FileInput& a, FileInput& b) noexcept;

	FileInput&
	operator=(FileInput&& other) noexcept;

	friend FileInput&
	operator>>(IO::File& file, FileInput& fileInput) noexcept;

	friend FileInput&
	operator>>(std::nullptr_t, FileInput& fileInput) noexcept;

	FileInput&
	seekGet(off_t pos) noexcept;

	[[nodiscard]] off_t
	tellGet() const noexcept;
};//class Stream::FileInput

/**
 * @brief	IO::File Output stream
 * @class	FileOutput File.hpp "Stream/File.hpp"
 */
class FileOutput : public Output {
	IO::File* mFile;
	off_t mOffset = 0;

	std::size_t
	writeBytes(std::byte const* src, std::size_t size) override;

	void
	flush() final;

public:
	struct Exception : Output::Exception
	{ using Output::Exception::Exception; };

	FileOutput() noexcept;

	FileOutput(FileOutput const&) = delete;

	FileOutput(FileOutput&& other) noexcept;

	friend void
	swap(FileOutput& a, FileOutput& b) noexcept;

	FileOutput&
	operator=(FileOutput&& other) noexcept;

	~FileOutput();

	friend FileOutput&
	operator<<(IO::File& file, FileOutput& fileOutput) noexcept;

	friend FileOutput&
	operator<<(std::nullptr_t, FileOutput& fileOutput) noexcept;

	FileOutput&
	seekPut(off_t pos);

	[[nodiscard]] off_t
	tellPut() const noexcept;
};
//class Stream::FileOutput

/**
 * @brief IO::File Input and Output stream
 * @class File File.hpp "Stream/File.hpp"
 */
class File : public FileInput, public FileOutput {};

void
swap(File& a, File& b) noexcept;

template <typename T>
concept FileInOut = std::derived_from<T, FileInput> && std::derived_from<T, FileOutput>;

auto&
operator<=>(IO::File& file, FileInOut auto& fileInOut) noexcept
{
	file >> fileInOut;
	file << fileInOut;
	return fileInOut;
}

auto&
operator<=>(std::nullptr_t, FileInOut auto& fileInOut) noexcept
{
	nullptr >> fileInOut;
	nullptr << fileInOut;
	return fileInOut;
}

}//namespace Stream

#endif //STREAM_FILE_HPP

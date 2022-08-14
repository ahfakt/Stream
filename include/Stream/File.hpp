#ifndef STREAM_FILE_HPP
#define STREAM_FILE_HPP

#include "InOut.hpp"
#include <fcntl.h>
#include <sys/types.h>

namespace Stream {

/**
 * @brief	File resource
 * @class	File File.hpp "Stream/File.hpp"
 */
class File : public Input, public Output {
	int mDescriptor = -1;

	std::size_t
	readBytes(std::byte* dest, std::size_t size) override;

	std::size_t
	writeBytes(std::byte const* src, std::size_t size) override;

	void
	flush() final;

public:
	struct Exception : std::system_error
	{ using std::system_error::system_error; };

	/**
	 * @brief	File open modes
	 * @class	Mode File.hpp "Stream/File.hpp"
	 */
	enum class Mode : int {
		R = O_RDONLY,
		W = O_WRONLY | O_CREAT | O_TRUNC,
		A = O_WRONLY | O_CREAT | O_APPEND,
		RW = O_RDWR,
		WR = O_RDWR | O_CREAT | O_TRUNC,
		AR = O_RDWR | O_CREAT | O_APPEND
	};//enum class Stream::File::Mode

	/**
	 * @brief	Construct a File resource.
	 * @throws	File::Exception
	 */
	File(std::string const& name, Mode mode);

	File(File const&) = delete;

	File(File&& other) noexcept;

	friend void
	swap(File& a, File& b) noexcept;

	File&
	operator=(File&& other) noexcept;

	~File();

	/**
	 * @brief	Get the block size of this file.
	 * @return	Block size in bytes
	 * @throws	File::Exception
	 */
	[[nodiscard]] blksize_t
	getBlockSize() const;

	/**
	 * @brief	Get the total size of this file.
	 * @return	File size in bytes
	 * @throws	File::Exception
	 */
	[[nodiscard]] off_t
	getFileSize() const;
};//class Stream::File

}//namespace Stream

#endif //STREAM_FILE_HPP

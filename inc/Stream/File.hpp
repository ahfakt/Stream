#pragma once

#include "InOut.hpp"
#include <expected>
#include <fcntl.h>
#include <sys/types.h>


namespace Stream {

/**
 * %File resource
 * @class	File File.hpp "Stream/File.hpp"
 */
class File : public Input, public Output {

	int mDescriptor;

	explicit
	File(int descriptor) noexcept;

	std::size_t
	readBytes(std::byte* dest, std::size_t size) final;

	std::size_t
	writeBytes(std::byte const* src, std::size_t size) final;

	void
	flush() final;

public:

	struct Exception : std::system_error
	{ using std::system_error::system_error; };

	/**
	 * File open modes
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
	 * Construct a %File resource.
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
	 * Get the block size of this file.
	 * @return	Block size in bytes
	 */
	[[nodiscard]]
	std::expected<::blksize_t, std::error_code>
	getBlockSize() const noexcept;

	/**
	 * Get the total size of this file.
	 * @return	%File size in bytes
	 */
	[[nodiscard]]
	std::expected<::off_t, std::error_code>
	getFileSize() const noexcept;

};//class Stream::File

}//namespace Stream

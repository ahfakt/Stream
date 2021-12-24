#pragma once

#include "InOut.hpp"
#include <climits>
#include <expected>

namespace Stream {

/**
 * %Pipe resource
 * @class	Pipe Pipe.hpp "Stream/Pipe.hpp"
 */
class Pipe : public Input, public Output {

	union {
		int mDescriptors[2];
		struct {
			int mReadDescriptor;
			int mWriteDescriptor;
		};
	};

	std::size_t
	readBytes(std::byte* dest, std::size_t size) final;

	std::size_t
	writeBytes(std::byte const* src, std::size_t size) final;

public:

	struct Exception : std::system_error
	{ using std::system_error::system_error; };

	/**
	 * Construct a %Pipe resource.
	 * @throws	Pipe::Exception
	 */
	Pipe();

	Pipe(Pipe const&) = delete;

	Pipe(Pipe&& other) noexcept;

	friend void
	swap(Pipe& a, Pipe& b) noexcept;

	Pipe&
	operator=(Pipe&& other) noexcept;

	~Pipe();

	/**
	 * Set the buffer size of the pipe.
	 * @param[in]	bufferSize Buffer size in bytes
	 * @return	Actual buffer size that is set
	 */
	[[nodiscard]]
	std::expected<std::size_t, std::error_code>
	setBufferSize(std::size_t bufferSize = PIPE_BUF) noexcept;

	/**
	 * Get the buffer size of the pipe.
	 * @return	Buffer size in bytes
	 */
	[[nodiscard]]
	std::expected<std::size_t, std::error_code>
	getBufferSize() const noexcept;

};//class Stream::Pipe

}//namespace Stream

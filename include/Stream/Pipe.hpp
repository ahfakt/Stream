#ifndef STREAM_PIPE_HPP
#define STREAM_PIPE_HPP

#include "InOut.hpp"
#include <climits>

namespace Stream {

/**
 * @brief	Pipe resource
 * @class	Pipe Pipe.hpp "Stream/Pipe.hpp"
 */
class Pipe : public Input, public Output {
	union {
		int mDescriptors[2] = {-1, -1};
		struct {
			int mRDescriptor;
			int mWDescriptor;
		};
	};

	std::size_t
	readBytes(std::byte* dest, std::size_t size) override;

	std::size_t
	writeBytes(std::byte const* src, std::size_t size) override;

public:
	struct Exception : std::system_error
	{ using std::system_error::system_error; };

	/**
	 * @brief	Construct a Pipe resource.
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
	 * @brief	Set the buffer size of the pipe.
	 * @param[in]	bufferSize Buffer size in bytes
	 * @return	Actual buffer size that is set
	 * @throws	Pipe::Exception
	 */
	std::size_t
	setBufferSize(std::size_t bufferSize = PIPE_BUF);

	/**
	 * @brief	Get the buffer size of the pipe.
	 * @return	Buffer size in bytes
	 * @throws	Pipe::Exception
	 */
	[[nodiscard]] std::size_t
	getBufferSize() const;
};//class Stream::Pipe

}//namespace Stream

#endif //STREAM_PIPE_HPP

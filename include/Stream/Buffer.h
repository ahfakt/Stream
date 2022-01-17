#ifndef STREAM_BUFFER_H
#define STREAM_BUFFER_H

#include "InOut.h"
#include <memory>

namespace Stream {

/**
 * @brief	Input stream buffer
 * @class	BufferInput Buffer.h "Stream/Buffer.h"
 */
class BufferInput : public InputFilter {
	std::unique_ptr<std::byte[]> mBuff;
	std::byte const* mEnd = nullptr;
	std::size_t mBuffSize = 0;
protected:
	std::byte const* mGet = nullptr;

	std::size_t
	readBytes(std::byte* dest, std::size_t size) override;

public:
	struct Exception : Input::Exception
	{ using Input::Exception::Exception; };

	explicit BufferInput(std::size_t buffInitialSize = 0);

	BufferInput(void const* sourceBuff, std::size_t sourceSize) noexcept;

	BufferInput(BufferInput&& other) noexcept;

	friend void
	swap(BufferInput& a, BufferInput& b) noexcept;

	BufferInput&
	operator=(BufferInput&& other) noexcept;

	[[nodiscard]] std::size_t
	getInputBufferSize() const noexcept;

	[[nodiscard]] std::size_t
	getAvailableDataSize() const noexcept;

	std::size_t
	provideData(std::size_t max);
};//class BufferInput

/**
 * @brief	Output stream buffer
 * @class	BufferOutput Buffer.h "Stream/Buffer.h"
 */
class BufferOutput : public OutputFilter {
	std::unique_ptr<std::byte[]> mBuff;
	std::byte const* mBeg = nullptr;
	std::byte const* mEnd = nullptr;
	std::size_t mBuffSize = 0;
protected:
	std::byte* mPut = nullptr;

	std::size_t
	writeBytes(std::byte const* src, std::size_t size) override;

	void
	flush() override;

public:
	struct Exception : Output::Exception
	{ using Output::Exception::Exception; };

	explicit BufferOutput(std::size_t buffInitialSize = 0);

	BufferOutput(void* sinkBuff, std::size_t sinkSize) noexcept;

	BufferOutput(BufferOutput&& other) noexcept;

	friend void
	swap(BufferOutput& a, BufferOutput& b) noexcept;

	BufferOutput&
	operator=(BufferOutput&& other) noexcept;

	~BufferOutput();

	[[nodiscard]] std::size_t
	getOutputBufferSize() const noexcept;

	[[nodiscard]] std::size_t
	getAvailableSpaceSize() const noexcept;

	std::size_t
	provideSpace(std::size_t min);
};//class BufferOutput

/**
 * @brief Input / Output stream buffer
 * @class Buffer Buffer.h "Stream/Buffer.h"
 */
class Buffer : public BufferInput, public BufferOutput {
public:
	struct Exception {
		enum class Code : int {
			BadAllocation = 1
		};
	};//struct Exception

	explicit Buffer(std::size_t inBuffInitialSize = 0, std::size_t outBuffInitialSize = 0);

	Buffer(std::size_t inBuffInitialSize, void* sinkBuff, std::size_t sinkSize);

	Buffer(void const* sourceBuff, std::size_t sourceSize, std::size_t outBuffInitialSize = 0);

	Buffer(void const* sourceBuff, std::size_t sourceSize, void* sinkBuff, std::size_t sinkSize) noexcept;
};//class Buffer

void
swap(Buffer& a, Buffer& b) noexcept;

std::error_code
make_error_code(Buffer::Exception::Code e) noexcept;

}//namespace Stream

namespace std {

template <>
struct is_error_code_enum<Stream::Buffer::Exception::Code> : true_type {};

}//namespace std

#endif //STREAM_BUFFER_H

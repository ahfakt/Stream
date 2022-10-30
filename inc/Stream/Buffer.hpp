#ifndef STREAM_BUFFER_HPP
#define STREAM_BUFFER_HPP

#include "Transparent.hpp"
#include <memory>

namespace Stream {

/**
 * @brief	Input stream buffer
 * @class	BufferInput Buffer.hpp "Stream/Buffer.hpp"
 */
class BufferInput : public TransparentInput {
protected:
	std::unique_ptr<std::byte[]> mBuff;
	std::byte const* mBeg{nullptr};
	std::byte const* mDataBeg{nullptr};
	std::byte* mDataEnd{nullptr};
	std::byte const* mEnd{nullptr};

	BufferInput() noexcept = default;

	std::size_t
	readBytes(std::byte* dest, std::size_t size) override;

public:
	struct Exception : Input::Exception
	{ using Input::Exception::Exception; };

	explicit BufferInput(std::size_t buffInitialSize);

	BufferInput(void const* sourceBuff, std::size_t sourceSize) noexcept;

	BufferInput(BufferInput&& other) noexcept;

	friend void
	swap(BufferInput& a, BufferInput& b) noexcept;

	BufferInput&
	operator=(BufferInput&& other) noexcept;

	[[nodiscard]] std::size_t
	getDataBufferSize() const noexcept;

	[[nodiscard]] std::size_t
	getDataSize() const noexcept;

	[[nodiscard]] std::byte const*
	getData() const noexcept;

	void
	advanceData(std::size_t size) noexcept;

	std::size_t
	provideSomeMoreData(std::size_t min);

	std::size_t
	provideSomeData(std::size_t max);

	void
	resetData() noexcept;
};//class Stream::BufferInput

/**
 * @brief	Output stream buffer
 * @class	BufferOutput Buffer.hpp "Stream/Buffer.hpp"
 */
class BufferOutput : public TransparentOutput {
protected:
	std::unique_ptr<std::byte[]> mBuff;
	std::byte const* mBeg{nullptr};
	std::byte* mSpaceBeg{nullptr};
	std::byte const* mSpaceEnd{nullptr};
	std::byte const* mEnd{nullptr};

	BufferOutput() noexcept = default;

	std::size_t
	writeBytes(std::byte const* src, std::size_t size) override;

public:
	struct Exception : Output::Exception
	{ using Output::Exception::Exception; };

	explicit BufferOutput(std::size_t buffInitialSize);

	BufferOutput(void* sinkBuff, std::size_t sinkSize) noexcept;

	BufferOutput(BufferOutput&& other) noexcept;

	friend void
	swap(BufferOutput& a, BufferOutput& b) noexcept;

	BufferOutput&
	operator=(BufferOutput&& other) noexcept;

	~BufferOutput();

	void
	flush() override;

	[[nodiscard]] std::size_t
	getSpaceBufferSize() const noexcept;

	[[nodiscard]] std::size_t
	getSpaceSize() const noexcept;

	std::byte*
	getSpace() noexcept;

	void
	advanceSpace(std::size_t size) noexcept;

	virtual std::size_t
	provideSpace(std::size_t min);

	virtual std::size_t
	provideSomeSpace(std::size_t max);

	void
	resetSpace() noexcept;
};//class Stream::BufferOutput

/**
 * @brief Input / Output stream buffer
 * @class Buffer Buffer.hpp "Stream/Buffer.hpp"
 */
class Buffer : public BufferInput, public BufferOutput {
public:
	struct Exception {
		enum class Code : int {
			BadAllocation = 1
		};
	};//struct Stream::Buffer::Exception

	explicit Buffer(std::size_t buffInitialSize);

	Buffer(std::size_t inBuffInitialSize, std::size_t outBuffInitialSize);

	Buffer(std::size_t inBuffInitialSize, void* sinkBuff, std::size_t sinkSize);

	Buffer(void const* sourceBuff, std::size_t sourceSize, std::size_t outBuffInitialSize);

	Buffer(void const* sourceBuff, std::size_t sourceSize, void* sinkBuff, std::size_t sinkSize) noexcept;
};//class Stream::Buffer

void
swap(Buffer& a, Buffer& b) noexcept;

std::error_code
make_error_code(Buffer::Exception::Code e) noexcept;

}//namespace Stream

namespace std {

template <>
struct is_error_code_enum<Stream::Buffer::Exception::Code> : true_type {};

}//namespace std

#endif //STREAM_BUFFER_HPP

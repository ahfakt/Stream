#pragma once

#include "InOut.hpp"
#include <memory>


namespace Stream {

/**
 * Input stream buffer
 * @class	BufferInput Buffer.hpp "Stream/Buffer.hpp"
 */
class BufferInput : public Input {
protected:

	std::unique_ptr<std::byte[]> mInputBuffer;
	std::size_t mInputBufferSize{0};
	std::byte const* mInputDataBeg{nullptr};
	std::byte* mInputDataEnd{nullptr};
	std::byte const* mInputEnd{nullptr};

	BufferInput() noexcept = default;

	std::size_t
	readBytes(std::byte* dest, std::size_t size) override;

	/**
	 * @param[in]	size
	 * @pre			@p size must be non-zero
	 * @throws		Input::Exception
	 */
	virtual std::size_t
	provideBytes(std::size_t size);

public:

	/**
	 * Construct with an initial buffer size
	 * @param[in]	initialBufferSize Number of bytes to allocate for input buffer
	 * @pre			@p initialBufferSize must be non-zero
	 * @throws		std::bad_alloc
	 */
	explicit
	BufferInput(std::size_t initialBufferSize);

	BufferInput(void const* sourceBuff, std::size_t sourceSize) noexcept;

	BufferInput(BufferInput&& other) noexcept;

	friend void
	swap(BufferInput& a, BufferInput& b) noexcept;

	BufferInput&
	operator=(BufferInput&& other) noexcept;

	/**
	 * @param[in]	size
	 * @throws		Input::Exception
	 */
	std::size_t
	provideSomeMore(std::size_t size);

	/**
	 * @param[in]	size
	 * @throws		Input::Exception
	 */
	std::size_t
	provideSome(std::size_t size);

	/**
	 * @param[in]	size
	 * @throws		Input::Exception
	 */
	std::size_t
	provide(std::size_t size);

	void
	consumed(std::size_t size) noexcept;

	[[nodiscard]]
	std::size_t
	getDataSize() const noexcept;

	std::byte const&
	operator[](int i) const noexcept;

	[[nodiscard]]
	std::byte const*
	begin() const noexcept;

	[[nodiscard]]
	std::byte const*
	end() const noexcept;

};//class Stream::BufferInput


/**
 * Output stream buffer
 * @class	BufferOutput Buffer.hpp "Stream/Buffer.hpp"
 */
class BufferOutput : public Output {
protected:

	std::unique_ptr<std::byte[]> mOutputBuffer;
	std::size_t mOutputBufferSize{0};
	std::byte const* mOutputDataBeg{nullptr};
	std::byte* mOutputDataEnd{nullptr};
	std::byte const* mOutputEnd{nullptr};

	BufferOutput() noexcept = default;

	std::size_t
	writeBytes(std::byte const* src, std::size_t size) override;

	void
	flush() override;

	virtual std::size_t
	allocBytes(std::size_t size);

public:

	/**
	 * Construct with an initial buffer size
	 * @param[in]	initialBufferSize Number of bytes to allocate for output buffer
	 * @pre			@p initialBufferSize must be non-zero
	 * @throws		std::bad_alloc
	 */
	explicit
	BufferOutput(std::size_t initialBufferSize);

	BufferOutput(void* sinkBuff, std::size_t sinkSize) noexcept;

	BufferOutput(BufferOutput&& other) noexcept;

	friend void
	swap(BufferOutput& a, BufferOutput& b) noexcept;

	BufferOutput&
	operator=(BufferOutput&& other) noexcept;

	virtual ~BufferOutput();

	/**
	 * @param[in]	size
	 * @throws		Output::Exception
	 */
	std::size_t
	allocSomeMore(std::size_t size);

	/**
	 * @param[in]	size
	 * @throws		Output::Exception
	 */
	std::size_t
	allocSome(std::size_t size);

	/**
	 * @param[in]	size
	 * @throws		Output::Exception
	 */
	std::size_t
	alloc(std::size_t size);

	void
	produced(std::size_t size) noexcept;

	[[nodiscard]]
	std::size_t
	getSpaceSize() const noexcept;

	[[nodiscard]]
	std::byte*
	begin() noexcept;

	[[nodiscard]]
	std::byte const*
	end() const noexcept;

};//class Stream::BufferOutput


/**
 * @brief Input and Output stream buffer
 * @class Buffer Buffer.hpp "Stream/Buffer.hpp"
 */
class Buffer : public BufferInput, public BufferOutput {
public:
	struct Exception {
		enum class Code : int {
			BadAllocation = 1
		};
	};//struct Stream::Buffer::Exception

	explicit
	Buffer(std::size_t buffInitialSize);

	Buffer(std::size_t inBuffInitialSize, std::size_t outBuffInitialSize);

	Buffer(std::size_t inBuffInitialSize, void* sinkBuff, std::size_t sinkSize);

	Buffer(void const* sourceBuff, std::size_t sourceSize, std::size_t outBuffInitialSize);

	Buffer(void const* sourceBuff, std::size_t sourceSize, void* sinkBuff, std::size_t sinkSize) noexcept;

	friend void
	swap(Buffer& a, Buffer& b) noexcept;

};//class Stream::Buffer


/**
 * BufferInput reader
 * @class	BufferReader Buffer.hpp "Stream/Buffer.hpp"
 */
class BufferReader : public Input {
	static BufferInput* Unreadable;

protected:

	BufferReader() noexcept;

	std::size_t
	readBytes(std::byte* dest, std::size_t size) override;

	BufferInput&
	getSource() noexcept;

public:

	/**
	 * Link @p bufferReader to @p bufferSource
	 */
	template <typename S, typename T>
	friend T&
	operator>(S& bufferSource, T& bufferReader) noexcept
	requires (std::derived_from<S, Input> && std::derived_from<S, BufferInput> &&
		std::derived_from<T, Input> && std::derived_from<T, BufferReader>);

	/**
	 * Disallow linking @p bufferReader to @p source
	 */
	template <typename S, typename T>
	friend T&
	operator>(S& source, T& bufferReader) noexcept
	requires (std::derived_from<S, Input> && !std::derived_from<S, BufferInput> &&
		std::derived_from<T, Input> && std::derived_from<T, BufferReader>
	) = delete/*("BufferReader needs a buffer and can not link to a source directly")*/;

	/**
	 * Link @p bufferReader to an unreadable buffer source
	 */
	template <typename T>
	friend T&
	operator>(std::nullptr_t, T& bufferReader) noexcept
	requires (std::derived_from<T, Input> && std::derived_from<T, BufferReader>);

};//class Stream::BufferReader


/**
 * BufferOutput writer
 * @class	BufferWriter Buffer.hpp "Stream/Buffer.hpp"
 */
 class BufferWriter : public Output {
	static BufferOutput* Unwritable;

protected:

	BufferWriter() noexcept;

	std::size_t
	writeBytes(std::byte const* src, std::size_t size) override;

	BufferOutput&
	getSink() noexcept;

public:

	/**
	 * Link @p bufferWriter to @p bufferSink
	 */
	template <typename S, typename T>
	friend T&
	operator<(S& bufferSink, T& bufferWriter) noexcept
	requires (std::derived_from<S, Output> && std::derived_from<S, BufferOutput> &&
		std::derived_from<T, Output> && std::derived_from<T, BufferWriter>);

	/**
	 * Disallow linking @p bufferWriter to @p sink
	 */
	template <typename S, typename T>
	friend T&
	operator<(S& sink, T& bufferWriter) noexcept
	requires (std::derived_from<S, Output> && !std::derived_from<S, BufferOutput> &&
		std::derived_from<T, Output> && std::derived_from<T, BufferWriter>
	) = delete/*("BufferWriter needs a buffer and can not link to a sink directly")*/;

	/**
	 * Link @p bufferWriter to an unwritable buffer sink
	 */
	template <typename T>
	friend T&
	operator<(std::nullptr_t, T& bufferWriter) noexcept
	requires (std::derived_from<T, Output> && std::derived_from<T, BufferWriter>);

};//class Stream::BufferWriter


std::error_code
make_error_code(Buffer::Exception::Code e) noexcept;

}//namespace Stream

namespace std {

template <>
struct is_error_code_enum<Stream::Buffer::Exception::Code> : true_type {};

}//namespace std


#include "../../src/Buffer.tpp"

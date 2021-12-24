#include "Stream/Buffer.hpp"
#include <cstring>
#include <unistd.h>


namespace Stream {

BufferInput::BufferInput(std::size_t initialBufferSize)
		: mInputBuffer{static_cast<std::byte*>(::operator new(initialBufferSize))}
		, mInputBufferSize{initialBufferSize}
		, mInputDataBeg{mInputBuffer.get()}
		, mInputDataEnd{mInputBuffer.get()}
		, mInputEnd{mInputBuffer.get() + initialBufferSize}
{}

BufferInput::BufferInput(void const* sourceBuff, std::size_t sourceSize) noexcept
		: mInputDataBeg{static_cast<std::byte const*>(sourceBuff)}
		, mInputDataEnd{static_cast<std::byte*>(const_cast<void*>(sourceBuff)) + sourceSize}
		, mInputEnd{static_cast<std::byte const*>(sourceBuff) + sourceSize}
{}

BufferInput::BufferInput(BufferInput&& other) noexcept
{ swap(*this, other); }

void
swap(BufferInput& a, BufferInput& b) noexcept
{
	swap(static_cast<Input&>(a), static_cast<Input&>(b));
	std::swap(a.mInputBuffer, b.mInputBuffer);
	std::swap(a.mInputBufferSize, b.mInputBufferSize);
	std::swap(a.mInputDataBeg, b.mInputDataBeg);
	std::swap(a.mInputDataEnd, b.mInputDataEnd);
	std::swap(a.mInputEnd, b.mInputEnd);
}

BufferInput&
BufferInput::operator=(BufferInput&& other) noexcept
{
	swap(*this, other);
	return *this;
}

std::size_t
BufferInput::readBytes(std::byte* dest, std::size_t size)
{
	size = provideSome(size);
	std::memcpy(dest, mInputDataBeg, size);
	consumed(size);
	return size;
}

std::size_t
BufferInput::provideBytes(std::size_t const size)
{
	if (size > mInputBufferSize) { // buffer is not large enough to fill with requested size of data
		auto newInputBufferSize{
			mInputBufferSize
				? ::ceilz(size, mInputBufferSize) * mInputBufferSize
				: size
		};
		auto* ptr{static_cast<std::byte*>(::operator new(newInputBufferSize, std::nothrow_t{}))};
		if (!ptr) [[unlikely]]
			throw Exception{Buffer::Exception::Code::BadAllocation};

		std::memcpy(ptr, mInputDataBeg, mInputDataEnd - mInputDataBeg);
		mInputBuffer.reset(ptr);
		mInputBufferSize = newInputBufferSize;
		mInputDataEnd += ptr - mInputDataBeg;
		mInputDataBeg = ptr;
		mInputEnd = ptr + newInputBufferSize;
	} else if (mInputDataBeg + size > mInputEnd || // shift the existing data if the remaining space is not enough
		mInputEnd - mInputDataEnd <= mInputDataBeg - mInputBuffer.get() // shift the existing data if the remaining space will at least double
	) {
		std::memcpy(mInputBuffer.get(), mInputDataBeg, mInputDataEnd - mInputDataBeg);
		mInputDataEnd += mInputBuffer.get() - mInputDataBeg;
		mInputDataBeg = mInputBuffer.get();
	}

	mInputDataEnd += getSource().readSome(mInputDataEnd, mInputEnd - mInputDataEnd);
	return std::min(size, static_cast<std::size_t>(mInputDataEnd - mInputDataBeg));
}

std::size_t
BufferInput::provideSomeMore(std::size_t const size)
{
	if (size)
		return provideBytes((mInputDataEnd - mInputDataBeg) + size);

	return mInputDataEnd - mInputDataBeg;
}

std::size_t
BufferInput::provideSome(std::size_t const size)
{
	if (size <= mInputDataEnd - mInputDataBeg) // there is enough data
		return size;

	if (mInputDataEnd != mInputDataBeg) // there is some data
		return mInputDataEnd - mInputDataBeg;

	return provideBytes(size);
}

std::size_t
BufferInput::provide(std::size_t const size)
{
	while (size > mInputDataEnd - mInputDataBeg) // there is not enough data
		provideBytes(size);

	return size;
}

void
BufferInput::consumed(std::size_t const size) noexcept
{ mInputDataBeg += size; }

std::size_t
BufferInput::getDataSize() const noexcept
{ return mInputDataEnd - mInputDataBeg; }

std::byte const&
BufferInput::operator[](int i) const noexcept
{ return *(mInputDataBeg + i); }

std::byte const*
BufferInput::begin() const noexcept
{ return mInputDataBeg; }

std::byte const*
BufferInput::end() const noexcept
{ return mInputDataEnd; }


BufferOutput::BufferOutput(std::size_t initialBufferSize)
		: mOutputBuffer{static_cast<std::byte*>(::operator new(initialBufferSize))}
		, mOutputBufferSize{initialBufferSize}
		, mOutputDataBeg{mOutputBuffer.get()}
		, mOutputDataEnd{mOutputBuffer.get()}
		, mOutputEnd{mOutputBuffer.get() + initialBufferSize}
{}

BufferOutput::BufferOutput(void* sinkBuff, std::size_t sinkSize) noexcept
		: mOutputDataBeg{static_cast<std::byte*>(sinkBuff)}
		, mOutputDataEnd{static_cast<std::byte*>(sinkBuff)}
		, mOutputEnd{static_cast<std::byte*>(sinkBuff) + sinkSize}
{}

BufferOutput::BufferOutput(BufferOutput&& other) noexcept
{ swap(*this, other); }

void
swap(BufferOutput& a, BufferOutput& b) noexcept
{
	swap(static_cast<Output&>(a), static_cast<Output&>(b));
	std::swap(a.mOutputBuffer, b.mOutputBuffer);
	std::swap(a.mOutputBufferSize, b.mOutputBufferSize);
	std::swap(a.mOutputDataBeg, b.mOutputDataBeg);
	std::swap(a.mOutputDataEnd, b.mOutputDataEnd);
	std::swap(a.mOutputEnd, b.mOutputEnd);
}

BufferOutput&
BufferOutput::operator=(BufferOutput&& other) noexcept
{
	swap(*this, other);
	return *this;
}

BufferOutput::~BufferOutput()
{
	try {
		getSink().write(mOutputDataBeg, mOutputDataEnd - mOutputDataBeg);
	} catch (Output::Exception const& exc) {
		// Nothing can be done
		LOG_ERR(exc.what())
	}
}

std::size_t
BufferOutput::writeBytes(std::byte const* src, std::size_t size)
{
	size = allocSome(size);
	std::memcpy(mOutputDataEnd, src, size);
	produced(size);
	return size;
}

void
BufferOutput::flush()
{
	try {
		getSink().write(mOutputDataBeg, mOutputDataEnd - mOutputDataBeg);
	} catch (Output::Exception const& exc) {
		mOutputDataBeg = static_cast<std::byte const*>(exc.getUnwrittenBuffer());
		throw;
	}
	mOutputDataBeg = const_cast<std::byte const*&>(mOutputDataEnd) = mOutputEnd - mOutputBufferSize;
}

std::size_t
BufferOutput::allocBytes(std::size_t const size)
{
	mOutputDataBeg += getSink().writeSome(mOutputDataBeg, mOutputDataEnd - mOutputDataBeg);

	if ((mOutputDataEnd - mOutputDataBeg) + size > mOutputBufferSize) {
		auto newOutputBufferSize{
			mOutputBufferSize
				? ::ceilz((mOutputDataEnd - mOutputDataBeg) + size, mOutputBufferSize) * mOutputBufferSize
				: (mOutputDataEnd - mOutputDataBeg) + size
		};
		auto* ptr{static_cast<std::byte*>(::operator new(newOutputBufferSize, std::nothrow_t{}))};
		if (!ptr) [[unlikely]]
			throw Exception{Buffer::Exception::Code::BadAllocation};

		std::memcpy(ptr, mOutputDataBeg, mOutputDataEnd - mOutputDataBeg);
		mOutputBuffer.reset(ptr);
		mOutputBufferSize = newOutputBufferSize;
		mOutputDataEnd += ptr - mOutputDataBeg;
		mOutputDataBeg = ptr;
		mOutputEnd = ptr + newOutputBufferSize;
	} else {
		std::memcpy(mOutputBuffer.get(), mOutputDataBeg, mOutputDataEnd - mOutputDataBeg);
		mOutputDataEnd += mOutputBuffer.get() - mOutputDataBeg;
		mOutputDataBeg = mOutputBuffer.get();
	}

	return std::min(size, static_cast<std::size_t>(mOutputEnd - mOutputDataEnd));
}

std::size_t
BufferOutput::allocSomeMore(std::size_t const size)
{
	if (size)
		return allocBytes((mOutputEnd - mOutputDataEnd) + size);

	return mOutputEnd - mOutputDataEnd;
}

std::size_t
BufferOutput::allocSome(std::size_t const size)
{
	if (size <= mOutputEnd - mOutputDataEnd) // there is enough space
		return size;

	if (mOutputEnd != mOutputDataEnd) // there is some space
		return mOutputEnd - mOutputDataEnd;

	return allocBytes(size);
}

std::size_t
BufferOutput::alloc(std::size_t const size)
{
	while (size > mOutputEnd - mOutputDataEnd) // there is not enough space
		allocBytes(size);

	return size;
}

void
BufferOutput::produced(std::size_t size) noexcept
{ mOutputDataEnd += size; }

std::size_t
BufferOutput::getSpaceSize() const noexcept
{ return mOutputEnd - mOutputDataEnd; }

std::byte*
BufferOutput::begin() noexcept
{ return mOutputDataEnd; }

std::byte const*
BufferOutput::end() const noexcept
{ return mOutputEnd; }


Buffer::Buffer(std::size_t buffInitialSize)
		: BufferInput{buffInitialSize}
		, BufferOutput{buffInitialSize}
{}

Buffer::Buffer(std::size_t inBuffInitialSize, std::size_t outBuffInitialSize)
		: BufferInput{inBuffInitialSize}
		, BufferOutput{outBuffInitialSize}
{}

Buffer::Buffer(std::size_t inBuffInitialSize, void* sinkBuff, std::size_t sinkSize)
		: BufferInput{inBuffInitialSize}
		, BufferOutput{sinkBuff, sinkSize}
{}

Buffer::Buffer(void const* sourceBuff, std::size_t sourceSize, std::size_t outBuffInitialSize)
		: BufferInput{sourceBuff, sourceSize}
		, BufferOutput{outBuffInitialSize}
{}

Buffer::Buffer(void const* sourceBuff, std::size_t sourceSize, void* sinkBuff, std::size_t sinkSize) noexcept
		: BufferInput{sourceBuff, sourceSize}
		, BufferOutput{sinkBuff, sinkSize}
{}

void
swap(Buffer& a, Buffer& b) noexcept
{
	swap(static_cast<BufferInput&>(a), static_cast<BufferInput&>(b));
	swap(static_cast<BufferOutput&>(a), static_cast<BufferOutput&>(b));
}


static class : public BufferInput {

	std::size_t
	readBytes(std::byte*, std::size_t) override
	{ throw Exception{std::make_error_code(std::errc::no_message_available)}; }

	std::size_t
	provideBytes(std::size_t) override
	{ throw Exception{std::make_error_code(std::errc::no_message_available)}; }

} unreadable;
BufferInput* BufferReader::Unreadable = &unreadable;

BufferReader::BufferReader() noexcept
{ *BufferReader::Unreadable > *this; }

size_t
BufferReader::readBytes(std::byte* dest, std::size_t size)
{ return getSource().readSome(dest, size); }

BufferInput&
BufferReader::getSource() noexcept
{ return reinterpret_cast<BufferInput&>(Input::getSource()); }


static class : public BufferOutput {

	std::size_t
	writeBytes(std::byte const*, std::size_t) override
	{ throw Exception{std::make_error_code(std::errc::no_space_on_device)}; }

	std::size_t
	allocBytes(std::size_t) override
	{ throw Exception{std::make_error_code(std::errc::no_space_on_device)}; }

} unwritable;
BufferOutput* BufferWriter::Unwritable = &unwritable;


BufferWriter::BufferWriter() noexcept
{ *BufferWriter::Unwritable < *this; }

std::size_t
BufferWriter::writeBytes(std::byte const* src, std::size_t size)
{ return getSink().writeSome(src, size); }

BufferOutput&
BufferWriter::getSink() noexcept
{ return reinterpret_cast<BufferOutput&>(Output::getSink()); }


std::error_code
make_error_code(Buffer::Exception::Code e) noexcept
{
	static struct : std::error_category {

		char const*
		name() const noexcept override
		{ return "Stream::Buffer"; }

		std::string
		message(int e) const noexcept override
		{
			using namespace std::string_literals;
			switch (static_cast<Buffer::Exception::Code>(e)) {
				case Buffer::Exception::Code::BadAllocation: return "Bad Allocation"s;
				default: return "Unknown Error"s;
			}
		}

	} const cat;

	return {static_cast<int>(e), cat};
}

}//namespace Stream

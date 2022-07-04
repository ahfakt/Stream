#include "Stream/Buffer.h"
#include <cstring>
#include <unistd.h>

namespace Stream {

BufferInput::BufferInput(std::size_t buffInitialSize)
		: mBuff(reinterpret_cast<std::byte*>(::operator new(buffInitialSize)))
		, mEnd(mBuff.get())
		, mBuffSize(buffInitialSize)
		, mGet(mBuff.get())
{}

BufferInput::BufferInput(void const* sourceBuff, std::size_t sourceSize) noexcept
		: mEnd(reinterpret_cast<std::byte const*>(sourceBuff) + sourceSize)
		, mBuffSize(sourceSize)
		, mGet(reinterpret_cast<std::byte const*>(sourceBuff))
{}

BufferInput::BufferInput(BufferInput&& other) noexcept
{ swap(*this, other); }

void
swap(BufferInput& a, BufferInput& b) noexcept
{
	swap(static_cast<InputFilter&>(a), static_cast<InputFilter&>(b));
	std::swap(a.mBuff, b.mBuff);
	std::swap(a.mEnd, b.mEnd);
	std::swap(a.mBuffSize, b.mBuffSize);
	std::swap(a.mGet, b.mGet);
}

BufferInput&
BufferInput::operator=(BufferInput&& other) noexcept
{
	std::swap(mBuff, other.mBuff);
	std::swap(mEnd, other.mEnd);
	std::swap(mBuffSize, other.mBuffSize);
	std::swap(mGet, other.mGet);
	return *this;
}

std::size_t
BufferInput::readBytes(std::byte* dest, std::size_t size)
{
	if (size > provideData(mBuffSize))
		size = getAvailableDataSize();
	std::memcpy(dest, mGet, size);
	mGet += size;
	return size;
}

std::size_t
BufferInput::getInputBufferSize() const noexcept
{ return mBuffSize; }

std::size_t
BufferInput::getAvailableDataSize() const noexcept
{ return mEnd - mGet; }

std::size_t
BufferInput::provideData(std::size_t max)
{
	if (mGet + max <= mEnd)
		return max;

	if (mGet != mEnd)
		return mEnd - mGet;

	if (max > mBuffSize) {
		if (auto* ptr = reinterpret_cast<std::byte*>(::operator new(max, std::nothrow_t{}))) {
			mBuff.reset(ptr);
			mBuffSize = max;
			mGet = mEnd = mBuff.get();
		} else
			throw Exception(Buffer::Exception::Code::BadAllocation);
	}

	max = mSource->readSome(mBuff.get(), max);
	mEnd = (mGet = mBuff.get()) + max;
	return max;
}

BufferOutput::BufferOutput(std::size_t buffInitialSize)
		: mBuff(reinterpret_cast<std::byte*>(::operator new(buffInitialSize)))
		, mBeg(mBuff.get())
		, mEnd(mBeg + buffInitialSize)
		, mBuffSize(buffInitialSize)
		, mPut(mBuff.get())
{}

BufferOutput::BufferOutput(void* sinkBuff, std::size_t sinkSize) noexcept
		: mBeg(reinterpret_cast<std::byte*>(sinkBuff))
		, mEnd(mBeg + sinkSize)
		, mBuffSize(sinkSize)
		, mPut(reinterpret_cast<std::byte*>(sinkBuff))
{}

BufferOutput::BufferOutput(BufferOutput&& other) noexcept
{ swap(*this, other); }

void
swap(BufferOutput& a, BufferOutput& b) noexcept
{
	swap(static_cast<OutputFilter&>(a), static_cast<OutputFilter&>(b));
	std::swap(a.mBuff, b.mBuff);
	std::swap(a.mBeg, b.mBeg);
	std::swap(a.mEnd, b.mEnd);
	std::swap(a.mBuffSize, b.mBuffSize);
	std::swap(a.mPut, b.mPut);
}

BufferOutput&
BufferOutput::operator=(BufferOutput&& other) noexcept
{
	std::swap(mBuff, other.mBuff);
	std::swap(mBeg, other.mBeg);
	std::swap(mEnd, other.mEnd);
	std::swap(mBuffSize, other.mBuffSize);
	std::swap(mPut, other.mPut);
	return *this;
}

std::size_t
BufferOutput::writeBytes(std::byte const* src, std::size_t size)
{
	if (size > provideSpace(1))
		size = getAvailableSpaceSize();
	std::memcpy(mPut, src, size);
	mPut += size;
	return size;
}

void
BufferOutput::flush()
{
	if (mPut != mBeg) try {
		mSink->write(mBeg, mPut - mBeg);
		mBeg = mBuff.get();
		mPut = mBuff.get();
	} catch (Output::Exception& exc) {
		mBeg = static_cast<std::byte const*>(exc.getUnwrittenBuffer());
		throw;
	}
}

BufferOutput::~BufferOutput()
{
	if (mPut != mBeg) try {
		mSink->write(mBeg, mPut - mBeg);
	} catch (Output::Exception& exc) {
		::write(STDERR_FILENO, exc.what(), std::strlen(exc.what()));
	}
}

std::size_t
BufferOutput::getOutputBufferSize() const noexcept
{ return mBuffSize; }

[[nodiscard]] std::size_t
BufferOutput::getAvailableSpaceSize() const noexcept
{ return mEnd - mPut; }

std::size_t
BufferOutput::provideSpace(std::size_t min)
{
	if (mPut + min <= mEnd)
		return mEnd - mPut;

	BufferOutput::flush();

	if (min > mBuffSize) {
		if (auto* ptr = reinterpret_cast<std::byte*>(::operator new(min, std::nothrow_t{}))) {
			mBuff.reset(ptr);
			mBeg = mBuff.get();
			mPut = mBuff.get();
			mEnd = mBeg + (mBuffSize = min);
		} else
			throw Exception(Buffer::Exception::Code::BadAllocation);
	}
	return mBuffSize;
}

void
BufferOutput::resetPut() noexcept
{ mPut = const_cast<std::byte*>(mBeg); }

Buffer::Buffer(std::size_t inBuffInitialSize, std::size_t outBuffInitialSize)
		: BufferInput(inBuffInitialSize)
		, BufferOutput(outBuffInitialSize)
{}

Buffer::Buffer(std::size_t inBuffInitialSize, void* sinkBuff, std::size_t sinkSize)
		: BufferInput(inBuffInitialSize)
		, BufferOutput(sinkBuff, sinkSize)
{}

Buffer::Buffer(void const* sourceBuff, std::size_t sourceSize, std::size_t outBuffInitialSize)
		: BufferInput(sourceBuff, sourceSize)
		, BufferOutput(outBuffInitialSize)
{}

Buffer::Buffer(void const* sourceBuff, std::size_t sourceSize, void* sinkBuff, std::size_t sinkSize) noexcept
		: BufferInput(sourceBuff, sourceSize)
		, BufferOutput(sinkBuff, sinkSize)
{}

void
swap(Buffer& a, Buffer& b) noexcept
{
	swap(static_cast<BufferInput&>(a), static_cast<BufferInput&>(b));
	swap(static_cast<BufferOutput&>(a), static_cast<BufferOutput&>(b));
}

std::error_code
make_error_code(Buffer::Exception::Code e) noexcept
{
	static struct : std::error_category {
		[[nodiscard]] char const*
		name() const noexcept override
		{ return "Stream::Buffer"; }

		[[nodiscard]] std::string
		message(int e) const noexcept override
		{ return e == 1 ? "Bad Allocation" : "Unknown Error"; }
	} instance;
	return {static_cast<int>(e), instance};
}

}//namespace Stream

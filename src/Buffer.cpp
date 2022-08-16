#include "Stream/Buffer.hpp"
#include <cstring>
#include <unistd.h>

namespace Stream {

BufferInput::BufferInput(std::size_t buffInitialSize)
		: mBuff(reinterpret_cast<std::byte*>(::operator new(buffInitialSize)))
		, mBeg(mBuff.get())
		, mDataBeg(mBeg)
		, mDataEnd(mBuff.get())
		, mEnd(mBeg + buffInitialSize)
{}

BufferInput::BufferInput(void const* sourceBuff, std::size_t sourceSize) noexcept
		: mBeg(reinterpret_cast<std::byte const*>(sourceBuff))
		, mDataBeg(mBeg)
		, mDataEnd(const_cast<std::byte*>(mDataBeg) + sourceSize)
		, mEnd(mBeg)
{}

BufferInput::BufferInput(BufferInput&& other) noexcept
{ swap(*this, other); }

void
swap(BufferInput& a, BufferInput& b) noexcept
{
	swap(static_cast<TransparentInput&>(a), static_cast<TransparentInput&>(b));
	std::swap(a.mBuff, b.mBuff);
	std::swap(a.mBeg, b.mBeg);
	std::swap(a.mDataBeg, b.mDataBeg);
	std::swap(a.mDataEnd, b.mDataEnd);
	std::swap(a.mEnd, b.mEnd);
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
	size = provideSomeData(size);
	std::memcpy(dest, mDataBeg, size);
	mDataBeg += size;
	return size;
}

std::size_t
BufferInput::getDataBufferSize() const noexcept
{ return mEnd - mBeg; }

std::size_t
BufferInput::getDataSize() const noexcept
{ return mDataEnd - mDataBeg; }

std::byte const*
BufferInput::getData() const noexcept
{ return mDataBeg; }

void
BufferInput::advanceData(std::size_t size) noexcept
{ mDataBeg += size; }

std::size_t
BufferInput::provideSomeMoreData(std::size_t const min)
{
	if (mDataEnd + min > mEnd) {
		auto const dataSize = mDataEnd - mDataBeg;
		if (mBeg + dataSize + min > mEnd) {
			if (auto* ptr = reinterpret_cast<std::byte*>(::operator new(dataSize + min, std::nothrow_t{}))) {
				std::memcpy(ptr, mDataBeg, dataSize);
				mDataEnd = ptr + dataSize;
				mDataBeg = ptr;
				mBuff.reset(ptr);
				mBeg = ptr;
				mEnd = ptr + dataSize + min;
			} else
				throw Exception(Buffer::Exception::Code::BadAllocation);
		} else {
			std::memmove(mBuff.get(), mDataBeg, dataSize);
			mDataEnd = mBuff.get() + dataSize;
			mDataBeg = mBuff.get();
		}
	}
	mDataEnd += getSome(mDataEnd, mEnd - mDataEnd);
	return mDataEnd - mDataBeg;
}

std::size_t
BufferInput::provideSomeData(std::size_t const max)
{
	if (mDataBeg + max > mDataEnd) {
		if (mDataBeg != mDataEnd)
			return mDataEnd - mDataBeg;
		if (mBeg + max > mEnd) {
			if (auto* ptr = reinterpret_cast<std::byte*>(::operator new(max, std::nothrow_t{}))) {
				mBuff.reset(ptr);
				mBeg = ptr;
				mEnd = ptr + max;
			} else
				throw Exception(Buffer::Exception::Code::BadAllocation);
		}
		mDataBeg = mBeg;
		mDataEnd = mBuff.get() + getSome(mBuff.get(), mEnd - mBeg);
		if (mDataBeg + max > mDataEnd)
			return mDataEnd - mDataBeg;
	}
	return max;
}

void
BufferInput::resetData() noexcept
{ mDataBeg = mBeg; }


BufferOutput::BufferOutput(std::size_t buffInitialSize)
		: mBuff(reinterpret_cast<std::byte*>(::operator new(buffInitialSize)))
		, mBeg(mBuff.get())
		, mSpaceBeg(mBuff.get())
		, mSpaceEnd(mSpaceBeg + buffInitialSize)
		, mEnd(mBeg + buffInitialSize)
{}

BufferOutput::BufferOutput(void* sinkBuff, std::size_t sinkSize) noexcept
		: mBeg(reinterpret_cast<std::byte*>(sinkBuff))
		, mSpaceBeg(reinterpret_cast<std::byte*>(sinkBuff))
		, mSpaceEnd(mSpaceBeg + sinkSize)
		, mEnd(mBeg)
{}

BufferOutput::BufferOutput(BufferOutput&& other) noexcept
{ swap(*this, other); }

void
swap(BufferOutput& a, BufferOutput& b) noexcept
{
	swap(static_cast<TransparentOutput&>(a), static_cast<TransparentOutput&>(b));
	std::swap(a.mBuff, b.mBuff);
	std::swap(a.mBeg, b.mBeg);
	std::swap(a.mSpaceBeg, b.mSpaceBeg);
	std::swap(a.mSpaceEnd, b.mSpaceEnd);
	std::swap(a.mEnd, b.mEnd);
}

BufferOutput&
BufferOutput::operator=(BufferOutput&& other) noexcept
{
	swap(*this, other);
	return *this;
}

BufferOutput::~BufferOutput()
{
	while (mBeg != mSpaceBeg) try {
		mBeg += putSome(mBeg, mSpaceBeg - mBeg);
	} catch (Output::Exception& exc) {
		::write(STDERR_FILENO, exc.what(), std::strlen(exc.what()));
		return;
	}
}

std::size_t
BufferOutput::writeBytes(std::byte const* src, std::size_t size)
{
	size = provideSomeSpace(size);
	std::memcpy(mSpaceBeg, src, size);
	mSpaceBeg += size;
	return size;
}

void
BufferOutput::flush()
{
	while (mBeg != mSpaceBeg) try {
		mBeg += putSome(mBeg, mSpaceBeg - mBeg);
	} catch (Output::Exception& exc) {
		mBeg = static_cast<std::byte const*>(exc.getUnwrittenBuffer());
		throw;
	}
	mBeg = mSpaceBeg = mBuff.get();
}

std::size_t
BufferOutput::getSpaceBufferSize() const noexcept
{ return mEnd - mBeg; }

[[nodiscard]] std::size_t
BufferOutput::getSpaceSize() const noexcept
{ return mSpaceEnd - mSpaceBeg; }

std::byte*
BufferOutput::getSpace() noexcept
{ return mSpaceBeg; }

void
BufferOutput::advanceSpace(std::size_t size) noexcept
{ mSpaceBeg += size; }

std::size_t
BufferOutput::provideSpace(std::size_t const min)
{
	if (mSpaceBeg + min > mSpaceEnd) {
		BufferOutput::flush();
		if (mBeg + min > mEnd) {
			if (auto* ptr = reinterpret_cast<std::byte*>(::operator new(min, std::nothrow_t{}))) {
				mBuff.reset(ptr);
				mBeg = mSpaceBeg = ptr;
				mEnd = mSpaceEnd = ptr + min;
			} else
				throw Exception(Buffer::Exception::Code::BadAllocation);
		}
	}
	return mSpaceEnd - mSpaceBeg;
}

std::size_t
BufferOutput::provideSomeSpace(std::size_t const max)
{
	if (mSpaceBeg + max > mSpaceEnd) {
		if (mSpaceBeg != mSpaceEnd)
			return mSpaceEnd - mSpaceBeg;
		BufferOutput::flush();
		if (mBeg + max > mEnd) {
			if (auto* ptr = reinterpret_cast<std::byte*>(::operator new(max, std::nothrow_t{}))) {
				mBuff.reset(ptr);
				mBeg = mSpaceBeg = ptr;
				mEnd = mSpaceEnd = ptr + max;
			} else
				throw Exception(Buffer::Exception::Code::BadAllocation);
		}
	}
	return max;
}

void
BufferOutput::resetSpace() noexcept
{ mSpaceBeg = const_cast<std::byte*>(mBeg); }

Buffer::Buffer(std::size_t buffInitialSize)
		: BufferInput(buffInitialSize)
		, BufferOutput(buffInitialSize)
{}

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

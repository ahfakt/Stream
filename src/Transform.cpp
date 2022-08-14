#include "Stream/Transform.hpp"

namespace Stream {

static class : public BufferInput {
	std::size_t
	readBytes(std::byte*, std::size_t) override
	{ throw Exception(std::make_error_code(static_cast<std::errc>(ENODATA))); }

	std::size_t
	provideData(std::size_t) override
	{ throw Exception(std::make_error_code(static_cast<std::errc>(ENODATA))); }

	std::size_t
	provideSomeData(std::size_t) override
	{ throw Exception(std::make_error_code(static_cast<std::errc>(ENODATA))); }
} nullBufferInput;

TransformInput::TransformInput() noexcept
		: mSource(&nullBufferInput)
{}

void
swap(TransformInput& a, TransformInput& b) noexcept
{ std::swap(a.mSource, b.mSource); }

size_t
TransformInput::readBytes(std::byte* dest, std::size_t size)
{ return mSource->readSome(dest, size); }

std::byte const*
TransformInput::getData() const noexcept
{ return mSource->getData(); }

void
TransformInput::advanceData(std::size_t size) noexcept
{ mSource->advanceData(size); }

std::size_t
TransformInput::provideData(std::size_t min)
{ return mSource->provideData(min); }

std::size_t
TransformInput::provideSomeData(std::size_t max)
{ return mSource->provideSomeData(max); }

TransformInput&
operator>(BufferInput& bufferInput, TransformInput& transformInput) noexcept
{
	transformInput.mSource = &bufferInput;
	return transformInput;
}

TransformInput&
operator>(std::nullptr_t, TransformInput& transformInput) noexcept
{ return nullBufferInput > transformInput; }

static class : public BufferOutput {
	std::size_t
	writeBytes(std::byte const* src, std::size_t size) override
	{ throw Exception(std::make_error_code(static_cast<std::errc>(ENOSPC))); }

	std::size_t
	provideSpace(std::size_t) override
	{ throw Exception(std::make_error_code(static_cast<std::errc>(ENOSPC))); }

	std::size_t
	provideSomeSpace(std::size_t) override
	{ throw Exception(std::make_error_code(static_cast<std::errc>(ENOSPC))); }
} nullBufferOutput;

TransformOutput::TransformOutput() noexcept
		: mSink(&nullBufferOutput)
{}

void
swap(TransformOutput& a, TransformOutput& b) noexcept
{ std::swap(a.mSink, b.mSink); }

std::size_t
TransformOutput::writeBytes(std::byte const* src, std::size_t size)
{ return mSink->writeSome(src, size); }

std::byte*
TransformOutput::getSpace() noexcept
{ return mSink->getSpace(); }

void
TransformOutput::advanceSpace(std::size_t size) noexcept
{ mSink->advanceSpace(size); }

std::size_t
TransformOutput::provideSpace(std::size_t min)
{ return mSink->provideSpace(min); }

std::size_t
TransformOutput::provideSomeSpace(std::size_t max)
{ return mSink->provideSomeSpace(max); }

void
TransformOutput::flush()
{ *mSink << nullptr; }

TransformOutput&
operator<(BufferOutput& bufferOutput, TransformOutput& transformOutput) noexcept
{
	transformOutput.mSink = &bufferOutput;
	return transformOutput;
}

TransformOutput&
operator<(std::nullptr_t, TransformOutput& transformOutput) noexcept
{ return nullBufferOutput < transformOutput; }

}//namespace Stream

#include "Stream/Transform.hpp"

namespace Stream {

static class : public BufferInput {
	std::size_t
	readBytes(std::byte*, std::size_t) override
	{ throw Exception{std::make_error_code(std::errc::no_message_available)}; }
} nullBufferInput;

TransformInput::TransformInput() noexcept
		: mSource{&nullBufferInput}
{}

TransformInput::TransformInput(TransformInput&& other) noexcept
		: TransformInput{}
{ swap(*this, other); }

void
swap(TransformInput& a, TransformInput& b) noexcept
{ std::swap(a.mSource, b.mSource); }

TransformInput&
TransformInput::operator=(TransformInput&& other) noexcept
{
	swap(*this, other);
	return *this;
}

size_t
TransformInput::readBytes(std::byte* dest, std::size_t size)
{ return mSource->readSome(dest, size); }

std::size_t
TransformInput::getDataSize() const noexcept
{ return mSource->getDataSize(); }

std::byte const*
TransformInput::getData() const noexcept
{ return mSource->getData(); }

void
TransformInput::advanceData(std::size_t size) noexcept
{ mSource->advanceData(size); }

std::size_t
TransformInput::provideSomeMoreData(std::size_t tryMin)
{ return mSource->provideSomeMoreData(tryMin); }

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
	writeBytes(std::byte const*, std::size_t) override
	{ throw Exception{std::make_error_code(std::errc::no_space_on_device)}; }

	void
	flush() override
	{ throw Exception{std::make_error_code(std::errc::no_space_on_device)}; }

	std::size_t
	provideSpace(std::size_t) override
	{ throw Exception{std::make_error_code(std::errc::no_space_on_device)}; }

	std::size_t
	provideSomeSpace(std::size_t) override
	{ throw Exception{std::make_error_code(std::errc::no_space_on_device)}; }
} nullBufferOutput;

TransformOutput::TransformOutput() noexcept
		: mSink{&nullBufferOutput}
{}

TransformOutput::TransformOutput(TransformOutput&& other) noexcept
		: TransformOutput{}
{ swap(*this, other); }

void
swap(TransformOutput& a, TransformOutput& b) noexcept
{ std::swap(a.mSink, b.mSink); }

TransformOutput&
TransformOutput::operator=(TransformOutput&& other) noexcept
{
	swap(*this, other);
	return *this;
}

std::size_t
TransformOutput::writeBytes(std::byte const* src, std::size_t size)
{ return mSink->writeSome(src, size); }

void
TransformOutput::flush()
{ mSink->flush(); }

std::size_t
TransformOutput::getSpaceSize() const noexcept
{ return mSink->getSpaceSize(); }

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

#include "Stream/Transparent.hpp"

namespace Stream {

static class : public Input {
	std::size_t
	readBytes(std::byte* dest, std::size_t size) override
	{ throw Exception(std::make_error_code(static_cast<std::errc>(ENODATA))); }
} nullInput;

TransparentInput::TransparentInput() noexcept
		: mSource(&nullInput)
{}

void
swap(TransparentInput& a, TransparentInput& b) noexcept
{ std::swap(a.mSource, b.mSource); }

std::size_t
TransparentInput::readBytes(std::byte* dest, std::size_t size)
{ return mSource->readSome(dest, size); }

std::size_t
TransparentInput::getSome(void* dest, std::size_t size)
{ return mSource->readSome(dest, size); }

TransparentInput&
operator>(Input& input, TransparentInput& transparentInput) noexcept
{
	transparentInput.mSource = &input;
	return transparentInput;
}

TransparentInput&
operator>(std::nullptr_t, TransparentInput& transparentInput) noexcept
{ return nullInput > transparentInput; }

static class : public Output {
	std::size_t
	writeBytes(std::byte const* src, std::size_t size) override
	{ throw Exception(std::make_error_code(static_cast<std::errc>(ENOSPC))); }

	void
	flush() override
	{ throw Exception(std::make_error_code(static_cast<std::errc>(ENOSPC))); }
} nullOutput;

TransparentOutput::TransparentOutput() noexcept
		: mSink(&nullOutput)
{}

void
swap(TransparentOutput& a, TransparentOutput& b) noexcept
{ std::swap(a.mSink, b.mSink); }

std::size_t
TransparentOutput::writeBytes(std::byte const* src, std::size_t size)
{ return mSink->writeSome(src, size); }

std::size_t
TransparentOutput::putSome(void const* src, std::size_t size)
{ return mSink->writeSome(src, size); }

TransparentOutput&
operator<(Output& output, TransparentOutput& transparentOutput) noexcept
{
	transparentOutput.mSink = &output;
	return transparentOutput;
}

TransparentOutput&
operator<(std::nullptr_t, TransparentOutput& transparentOutput) noexcept
{ return nullOutput < transparentOutput; }

}//namespace Stream

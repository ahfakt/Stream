#include "Stream/Transparent.hpp"

namespace Stream {

static class : public Input {
	std::size_t
	readBytes(std::byte* dest, std::size_t size) override
	{ throw Exception(std::make_error_code(std::errc::no_message_available)); }
} nullInput;

TransparentInput::TransparentInput() noexcept
		: mSource(&nullInput)
{}

TransparentInput::TransparentInput(TransparentInput&& other) noexcept
{ swap(*this, other); }

void
swap(TransparentInput& a, TransparentInput& b) noexcept
{ std::swap(a.mSource, b.mSource); }

TransparentInput&
TransparentInput::operator=(TransparentInput&& other) noexcept
{
	swap(*this, other);
	return *this;
}

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
	{ throw Exception(std::make_error_code(std::errc::no_space_on_device)); }

	void
	flush() override
	{ throw Exception(std::make_error_code(std::errc::no_space_on_device)); }
} nullOutput;

TransparentOutput::TransparentOutput() noexcept
		: mSink(&nullOutput)
{}

TransparentOutput::TransparentOutput(TransparentOutput&& other) noexcept
{ swap(*this, other); }

void
swap(TransparentOutput& a, TransparentOutput& b) noexcept
{ std::swap(a.mSink, b.mSink); }

TransparentOutput&
TransparentOutput::operator=(TransparentOutput&& other) noexcept
{
	swap(*this, other);
	return *this;
}

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

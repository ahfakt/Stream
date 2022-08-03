#include "Stream/Output.hpp"

namespace Stream {

void
Output::flush()
{}

Output&
Output::write(void const* src, std::size_t size)
try {
	for (std::size_t inl; size; reinterpret_cast<std::byte const*&>(src) += inl, size -= inl)
		inl = writeBytes(reinterpret_cast<std::byte const*>(src), size);
	return *this;
} catch (Exception& exc) {
	exc.mSrc = src;
	exc.mSize = size;
	throw;
}

std::size_t
Output::writeSome(void const* src, std::size_t size)
{
	std::size_t inl;
	while (!(inl = writeBytes(reinterpret_cast<std::byte const*>(src), size)));
	return inl;
}

Output&
Output::operator<<(std::nullptr_t)
{
	flush();
	return *this;
}

Output&
Output::operator<<(std::string const& str)
{ return write(str.c_str(), str.size() + 1); }

Output&
Output::operator<<(char const* str)
{ return write(str, std::strlen(str) + 1); }

void const*
Output::Exception::getUnwrittenBuffer() const noexcept
{ return mSrc; }

std::size_t
Output::Exception::getUnwrittenSize() const noexcept
{ return mSize; }

OutputFilter::OutputFilter() noexcept
{ nullptr << *this; }

void
swap(OutputFilter& a, OutputFilter& b) noexcept
{ std::swap(a.mSink, b.mSink); }

std::size_t
OutputFilter::writeBytes(std::byte const* src, std::size_t size)
{ return mSink->writeSome(src, size); }

OutputFilter&
operator<<(Output& output, OutputFilter& outputFilter) noexcept
{
	outputFilter.mSink = &output;
	return outputFilter;
}

OutputFilter&
operator<<(std::nullptr_t, OutputFilter& outputFilter) noexcept
{
	static class : public Output {
		std::size_t
		writeBytes(std::byte const* src, std::size_t size) override
		{ throw Exception(std::make_error_code(static_cast<std::errc>(ENOSPC))); }
	} nullOutput;
	return nullOutput << outputFilter;
}

std::error_code
make_error_code(Output::Exception::Code e) noexcept
{
	static struct : std::error_category {
		[[nodiscard]] char const*
		name() const noexcept override
		{ return "Stream::Output"; }

		[[nodiscard]] std::string
		message(int e) const noexcept override
		{ return e == 1 ? "Uninitialized" : "Unknown Error"; }
	} instance;
	return {static_cast<int>(e), instance};
}

}//namespace Stream

#include "Stream/InOut.h"
#include <cstring>

namespace Stream {

Input&
Input::read(void* dest, std::size_t size)
try {
	for (std::size_t outl; size; reinterpret_cast<std::byte*&>(dest) += outl, size -= outl)
		outl = readBytes(reinterpret_cast<std::byte*>(dest), size);
	return *this;
} catch (Exception& exc) {
	exc.mDest = dest;
	exc.mSize = size;
	throw;
}

std::size_t
Input::readSome(void* dest, std::size_t size)
{
	std::size_t outl;
	while (!(outl = readBytes(reinterpret_cast<std::byte*>(dest), size)));
	return outl;
}

void*
Input::Exception::getUnreadBuffer() const noexcept
{ return mDest; }

std::size_t
Input::Exception::getUnreadSize() const noexcept
{ return mSize; }

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

void const*
Output::Exception::getUnwrittenBuffer() const noexcept
{ return mSrc; }

std::size_t
Output::Exception::getUnwrittenSize() const noexcept
{ return mSize; }

InputFilter::InputFilter() noexcept
{ nullptr >> *this; }

void
swap(InputFilter& a, InputFilter& b) noexcept
{ std::swap(a.mSource, b.mSource); }

InputFilter&
operator>>(Input& input, InputFilter& inputFilter) noexcept
{
	inputFilter.mSource = &input;
	return inputFilter;
}

InputFilter&
operator>>(std::nullptr_t, InputFilter& inputFilter) noexcept
{
	static class : public Input {
		std::size_t
		readBytes(std::byte* dest, std::size_t size) override
		{ throw Exception(std::make_error_code(static_cast<std::errc>(ENODATA))); }
	} instance;
	inputFilter.mSource = &instance;
	return inputFilter;
}

OutputFilter::OutputFilter() noexcept
{ nullptr << *this; }

void
swap(OutputFilter& a, OutputFilter& b) noexcept
{ std::swap(a.mSink, b.mSink); }

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
	} instance;
	outputFilter.mSink = &instance;
	return outputFilter;
}

Input&
operator>>(Input& input, std::string& str)
{
	str.clear();
	char c;
	input.read(&c, 1);
	while (c) {
		str.push_back(c);
		input.read(&c, 1);
	}
	return input;
}

Input&
operator>>(Input& input, char* str)
{
	do {
		input.read(str, 1);
	} while(*str++);
	return input;
}

Output&
operator<<(Output& output, std::string const& str)
{ return output.write(str.c_str(), str.size() + 1); }

Output&
operator<<(Output& output, char const* str)
{ return output.write(str, std::strlen(str) + 1); }

std::error_code
make_error_code(Input::Exception::Code e) noexcept
{
	static struct : std::error_category {
		[[nodiscard]] char const*
		name() const noexcept override
		{ return "Stream::Input"; }

		[[nodiscard]] std::string
		message(int ev) const noexcept override
		{ return ev == 1 ? "Uninitialized" : "Unknown Error"; }
	} instance;
	return {static_cast<int>(e), instance};
}

std::error_code
make_error_code(Output::Exception::Code e) noexcept
{
	static struct : std::error_category {
		[[nodiscard]] char const*
		name() const noexcept override
		{ return "Stream::Output"; }

		[[nodiscard]] std::string
		message(int ev) const noexcept override
		{ return ev == 1 ? "Uninitialized" : "Unknown Error"; }
	} instance;
	return {static_cast<int>(e), instance};
}

}//namespace Stream

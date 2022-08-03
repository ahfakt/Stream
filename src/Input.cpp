#include "Stream/Input.hpp"

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

Input&
Input::operator>>(std::string& str)
{
	str.clear();
	char c;
	read(&c, 1);
	while (c) {
		str.push_back(c);
		read(&c, 1);
	}
	return *this;
}

Input&
Input::operator>>(char* str)
{
	do {
		read(str, 1);
	} while(*str++);
	return *this;
}

void*
Input::Exception::getUnreadBuffer() const noexcept
{ return mDest; }

std::size_t
Input::Exception::getUnreadSize() const noexcept
{ return mSize; }

InputFilter::InputFilter() noexcept
{ nullptr >> *this; }

void
swap(InputFilter& a, InputFilter& b) noexcept
{ std::swap(a.mSource, b.mSource); }

std::size_t
InputFilter::readBytes(std::byte* dest, std::size_t size)
{ return mSource->readSome(dest, size); }

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
	} nullInput;
	return nullInput >> inputFilter;
}

std::error_code
make_error_code(Input::Exception::Code e) noexcept
{
	static struct : std::error_category {
		[[nodiscard]] char const*
		name() const noexcept override
		{ return "Stream::Input"; }

		[[nodiscard]] std::string
		message(int e) const noexcept override
		{ return e == 1 ? "Uninitialized" : "Unknown Error"; }
	} instance;
	return {static_cast<int>(e), instance};
}

}//namespace Stream

#include "Stream/Input.h"

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

InputFilter::InputFilter() noexcept
{ nullptr >> *this; }

void
swap(InputFilter& a, InputFilter& b) noexcept
{ std::swap(a.mSource, b.mSource); }

std::size_t
InputFilter::readBytes(std::byte* dest, std::size_t size)
{ return mSource->readSome(dest, size); }

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

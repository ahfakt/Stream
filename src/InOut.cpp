#include "Stream/InOut.hpp"
#include <unistd.h>

namespace Stream {

Input&
Input::read(void* dest, std::size_t size)
try {
	while (size) {
		auto outl{readBytes(reinterpret_cast<std::byte*>(dest), size)};
		reinterpret_cast<std::byte*&>(dest) += outl;
		size -= outl;
	}
	return *this;
} catch (Exception& exc) {
	exc.mDest = dest;
	exc.mSize = size;
	throw;
}

std::size_t
Input::readSome(void* dest, std::size_t size)
{
	if (size) do {
		if (auto outl{readBytes(reinterpret_cast<std::byte*>(dest), size)})
			return outl;
	} while (true);
	return 0;
}

void*
Input::Exception::getUnreadBuffer() const noexcept
{ return mDest; }

std::size_t
Input::Exception::getUnreadSize() const noexcept
{ return mSize; }

static class : public Input {
	std::size_t
	readBytes(std::byte* dest, std::size_t size) override
	{
		while (true) {
			auto r{::read(STDIN_FILENO, dest, size)};
			if (r > 0)
				return r;
			if (r == 0)
				throw Input::Exception{std::make_error_code(std::errc::no_message_available)};
			if (errno != EINTR)
				throw Input::Exception{std::make_error_code(static_cast<std::errc>(errno))};
		}
	}
} StdIn;
Input& In = StdIn;

Output&
Output::write(void const* src, std::size_t size)
try {
	while (size) {
		auto inl{writeBytes(reinterpret_cast<std::byte const*>(src), size)};
		reinterpret_cast<std::byte const*&>(src) += inl;
		size -= inl;
	}
	return *this;
} catch (Exception& exc) {
	exc.mSrc = src;
	exc.mSize = size;
	throw;
}

std::size_t
Output::writeSome(void const* src, std::size_t size)
{
	if (size) do {
		if (auto inl{writeBytes(reinterpret_cast<std::byte const*>(src), size)})
			return inl;
	} while (true);
	return 0;
}

void
Output::flush()
{}

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

static class : public Output {
	std::size_t
	writeBytes(std::byte const* src, std::size_t size) override
	{
		while (true) {
			if (auto r{::write(STDOUT_FILENO, src, size)}; r >= 0)
				return r;
			if (errno != EINTR)
				throw Output::Exception{std::make_error_code(static_cast<std::errc>(errno))};
		}
	}
} StdOut;
Output& Out = StdOut;

static class : public Output {
	std::size_t
	writeBytes(std::byte const* src, std::size_t size) override
	{
		while (true) {
			if (auto r{::write(STDERR_FILENO, src, size)}; r >= 0)
				return r;
			if (errno != EINTR)
				throw Output::Exception{std::make_error_code(static_cast<std::errc>(errno))};
		}
	}
} StdErr;
Output& Err = StdErr;

std::error_code
make_error_code(Input::Exception::Code e) noexcept
{
	static struct : std::error_category {
		[[nodiscard]] char const*
		name() const noexcept override
		{ return "Stream::Input"; }

		[[nodiscard]] std::string
		message(int e) const noexcept override
		{
			using namespace std::string_literals;
			return static_cast<Input::Exception::Code>(e) == Input::Exception::Code::Uninitialized
				? "Uninitialized"s : "Unknown Error"s;
		}
	} const cat;
	return {static_cast<int>(e), cat};
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
		{
			using namespace std::string_literals;
			return static_cast<Output::Exception::Code>(e) == Output::Exception::Code::Uninitialized
				? "Uninitialized"s : "Unknown Error"s;
		}
	} const cat;
	return {static_cast<int>(e), cat};
}

}//namespace Stream

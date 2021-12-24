#include "Stream/InOut.hpp"
#include <unistd.h>


namespace Stream {

Input::Input(bool allowLink) noexcept
		: mSource{allowLink ? Input::Unreadable : nullptr}
{}

void
Input::drain()
{}

Input&
Input::getSource() noexcept
{ return *mSource; }

Input::Input(Input&& other) noexcept
		: Input()
{ swap(*this, other); }

void
swap(Input& a, Input& b) noexcept
{ std::swap(a.mSource, b.mSource); }

Input&
Input::operator=(Input&& other) noexcept
{
	swap(*this, other);
	return *this;
}

Input&
Input::operator>>(std::nullptr_t)
{
	drain();
	return *this;
}

Input&
Input::operator>(std::nullptr_t)
{
	// subclass can explicitly disallow linking to another source
	// do not trigger the mSource if it is unreadable to avoid infinite recursion
	if (mSource && mSource != Input::Unreadable)
		getSource() > nullptr;
	drain();
	return *this;
}

Input&
Input::read(void* dest, std::size_t size)
{
	try {
		while (size) {
			std::size_t outl;
			while (!(outl = readBytes(reinterpret_cast<std::byte*>(dest), size)));
			reinterpret_cast<std::byte*&>(dest) += outl;
			size -= outl;
		}
		return *this;
	} catch (Input::Exception& exc) {
		exc.mDest = dest;
		exc.mSize = size;
		throw;
	}
}

std::size_t
Input::readSome(void* dest, std::size_t size)
{
	std::size_t outl{0};
	if (size)
		while (!(outl = readBytes(reinterpret_cast<std::byte*>(dest), size)));
	return outl;
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
	{ throw Input::Exception{std::make_error_code(std::errc::no_message_available)}; }

} unreadable;
Input* Input::Unreadable = &unreadable;


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

} stdIn;
Input& StdIn = stdIn;


Output::Output(bool allowLink) noexcept
		: mSink{allowLink ? Output::Unwritable : nullptr}
{}

void
Output::flush()
{}

Output&
Output::getSink() noexcept
{ return *mSink; }

Output::Output(Output&& other) noexcept
		: Output{}
{ swap(*this, other); }

void
swap(Output& a, Output& b) noexcept
{ std::swap(a.mSink, b.mSink); }

Output&
Output::operator=(Output&& other) noexcept
{
	swap(*this, other);
	return *this;
}

Output&
Output::operator<<(std::nullptr_t)
{
	flush();
	return *this;
}

Output&
Output::operator<(std::nullptr_t)
{
	flush();
	// subclass can explicitly disallow linking to another sink
	// do not trigger the mSink if it is unwritable to avoid infinite recursion
	if (mSink && mSink != Output::Unwritable)
		getSink() < nullptr;
	return *this;
}

Output&
Output::write(void const* src, std::size_t size)
{
	try {
		while (size) {
			std::size_t inl;
			while (!(inl = writeBytes(reinterpret_cast<std::byte const*>(src), size)));
			reinterpret_cast<std::byte const*&>(src) += inl;
			size -= inl;
		}
		return *this;
	} catch (Output::Exception& exc) {
		exc.mSrc = src;
		exc.mSize = size;
		throw;
	}
}

std::size_t
Output::writeSome(void const* src, std::size_t size)
{
	std::size_t inl{0};
	if (size)
		while (!(inl = writeBytes(reinterpret_cast<std::byte const*>(src), size)));
	return inl;
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
	{ throw Output::Exception{std::make_error_code(std::errc::no_space_on_device)}; }

} unwritable;
Output* Output::Unwritable = &unwritable;


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

} stdOut;
Output& StdOut = stdOut;


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

} stdErr;
Output& StdErr = stdErr;


std::error_code
make_error_code(Exception::Code e) noexcept
{
	static struct : std::error_category {

		char const*
		name() const noexcept override
		{ return "Stream"; }

		std::string
		message(int e) const noexcept override
		{
			using namespace std::string_literals;
			switch (static_cast<Exception::Code>(e)) {
				case Exception::Code::Uninitialized: return "Uninitialized"s;
				default: return "Unknown Error"s;
			}
		}

	} const cat;

	return {static_cast<int>(e), cat};
}

}//namespace Stream

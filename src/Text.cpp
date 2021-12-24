#include "Stream/Text.hpp"

namespace Stream {

TextInput&
TextInput::checkFromChars(std::from_chars_result r)
{
	if (r.ec != std::errc{}) [[unlikely]]
		throw Exception{std::make_error_code(r.ec)};
	getSource().consumed(reinterpret_cast<std::byte const*>(r.ptr) - getSource().begin());
	return *this;
}

std::size_t
TextInput::provideFloat(std::size_t const start, std::chars_format const fmt, std::size_t const intLimit, std::size_t const fracLimit, std::size_t const expLimit)
{
	auto i{start};
	try {
		if (i == getSource().getDataSize())
			getSource().provideSomeMore(1);
		if (getSource()[i] == std::byte{'-'}) // optional
			++i;
		if (fmt != std::chars_format::hex) {
			if (auto r{provideDigits10(i, intLimit)}) try {
				i += provideDecFrac(i += r, fracLimit);
				if (fmt != std::chars_format::fixed)
					i += provideDecExp(i, expLimit);
			} catch (Input::Exception const& exc) {
				if (exc.code() != std::make_error_code(std::errc::no_message_available))
					throw;
			}
		} else {
			if (auto r{provideDigits36(i, intLimit)}) try {
				i += provideHexFrac(i += r, fracLimit);
				i += provideHexExp(i, expLimit);
			} catch (Input::Exception const& exc) {
				if (exc.code() != std::make_error_code(std::errc::no_message_available))
					throw;
			}
		}
		return i - start;
	} catch (Input::Exception const& exc) {
		if (exc.code() != std::make_error_code(std::errc::no_message_available) || i == start)
			throw;
	}
	return 0;
}

std::size_t
TextInput::provideSignedInt(std::size_t const start, std::size_t const limit, unsigned const base)
{
	auto i{start};
	try {
		if (i == getSource().getDataSize())
			getSource().provideSomeMore(1);
		if (getSource()[i] == std::byte{'-'}) // optional
			++i;
		if (auto r{provideUnsignedInt(i, limit, base)})
			return i - start + r;
	} catch (Input::Exception const& exc) {
		if (exc.code() != std::make_error_code(std::errc::no_message_available) || i == start)
			throw;
	}
	return 0;
}

std::size_t
TextInput::provideUnsignedInt(std::size_t const start, std::size_t const limit, unsigned const base)
{ return base <= 10 ? provideDigits10(start, limit, base) : provideDigits36(start, limit, base); }

std::size_t
TextInput::provideDecFrac(std::size_t const start, std::size_t const fracLimit)
{
	auto i{start};
	try {
		if (i == getSource().getDataSize())
			getSource().provideSomeMore(1);
		if (getSource()[i] == std::byte{'.'}) // required
			if (auto r{provideDigits10(++i, fracLimit)})
				return i - start + r;
	} catch (Input::Exception const& exc) {
		if (exc.code() != std::make_error_code(std::errc::no_message_available) || i == start)
			throw;
	}
	return 0;
}

std::size_t
TextInput::provideDecExp(std::size_t const start, std::size_t const expLimit)
{
	auto i{start};
	try {
		if (i == getSource().getDataSize())
			getSource().provideSomeMore(1);
		if (getSource()[i] == std::byte{'e'} || getSource()[i] == std::byte{'E'}) { // required
			if (++i == getSource().getDataSize())
				getSource().provideSomeMore(1);
			if (getSource()[i] == std::byte{'-'} || getSource()[i] == std::byte{'+'}) // required
				if (auto r{provideDigits10(++i, expLimit)})
					return i - start + r;
		}
	} catch (Input::Exception const& exc) {
		if (exc.code() != std::make_error_code(std::errc::no_message_available) || i == start)
			throw;
	}
	return 0;
}

std::size_t
TextInput::provideDigits10(std::size_t const start, std::size_t const limit, unsigned const base)
{
	auto i{start};
	char const d = '0' + base;
	while (true) {
		char const* s{reinterpret_cast<char const*>(getSource().begin())};
		for (auto const e{getSource().getDataSize()}; i < e; ++i) {
			if (!(s[i] >= '0' && s[i] < d))
				return i - start;
			if (i - start == limit)
				throw Exception{std::make_error_code(std::errc::result_out_of_range)};
		}
		try {
			getSource().provideSomeMore(1);
		} catch (Input::Exception const& exc) {
			if (exc.code() != std::make_error_code(std::errc::no_message_available) || i == start)
				throw;
			return i - start;
		}
	}
}

std::size_t
TextInput::provideHexFrac(std::size_t const start, std::size_t const fracLimit)
{
	auto i{start};
	try {
		if (i == getSource().getDataSize())
			getSource().provideSomeMore(1);
		if (getSource()[i] == std::byte{'.'}) // required
			if (auto r{provideDigits36(++i, fracLimit)})
				return i - start + r;
	} catch (Input::Exception const& exc) {
		if (exc.code() != std::make_error_code(std::errc::no_message_available) || i == start)
			throw;
	}
	return 0;
}

std::size_t
TextInput::provideHexExp(std::size_t const start, std::size_t const expLimit)
{
	auto i{start};
	try {
		if (i == getSource().getDataSize())
			getSource().provideSomeMore(1);
		if (getSource()[i] == std::byte{'p'} || getSource()[i] == std::byte{'P'}) { // required
			if (++i == getSource().getDataSize())
				getSource().provideSomeMore(1);
			if (getSource()[i] == std::byte{'-'} || getSource()[i] == std::byte{'+'}) // required
				if (auto r{provideDigits36(++i, expLimit)})
					return i - start + r;
		}
	} catch (Input::Exception const& exc) {
		if (exc.code() != std::make_error_code(std::errc::no_message_available) || i == start)
			throw;
	}
	return 0;
}

std::size_t
TextInput::provideDigits36(std::size_t const start, std::size_t const limit, unsigned const base)
{
	auto i{start};
	char const d = 'a' + base - 10;
	char const D = 'A' + base - 10;
	while (true) {
		char const* s{reinterpret_cast<char const*>(getSource().begin())};
		for (auto const e{getSource().getDataSize()}; i < e; ++i) {
			if (!(s[i] >= '0' && s[i] <= '9' || s[i] >= 'a' && s[i] < d || s[i] >= 'A' && s[i] < D))
				return i - start;
			if (i - start == limit)
				throw Exception{std::make_error_code(std::errc::result_out_of_range)};
		}
		try {
			getSource().provideSomeMore(1);
		} catch (Input::Exception const& exc) {
			if (exc.code() != std::make_error_code(std::errc::no_message_available) || i == start)
				throw;
			return i - start;
		}
	}
}

std::pair<std::size_t, std::size_t const>
TextInput::provideUntil(std::size_t const start, std::size_t const limit, char const delim)
{
	auto i{start};
	while (true) {
		char const* s{reinterpret_cast<char const*>(getSource().begin())};
		for (auto const e {getSource().getDataSize()}; i < e; ++i) {
			if (s[i] == delim)
				return {i - start, i - start + 1};
			if (i - start == limit)
				throw Exception{std::make_error_code(std::errc::result_out_of_range)};
		}
		try {
			getSource().provideSomeMore(1);
		} catch (Input::Exception const& exc) {
			if (exc.code() != std::make_error_code(std::errc::no_message_available) || i == start)
				throw;
			return {i - start, i - start};
		}
	}
}

std::string_view
TextInput::getLine(std::size_t const limit)
{
	auto [len, size]{provideUntil(0, limit, '\n')};
	if (len && len < size && getSource()[len - 1] == std::byte{'\r'})
		--len;
	char const* str{reinterpret_cast<char const*>(getSource().begin())};
	getSource().consumed(size);
	return {str, len};
}

std::string_view
TextInput::getUntil(char const delim, std::size_t const limit)
{
	auto [len, size]{provideUntil(0, limit, delim)};
	char const* str{reinterpret_cast<char const*>(getSource().begin())};
	getSource().consumed(size);
	return {str, len};
}

TextOutput&
TextOutput::checkToChars(std::to_chars_result const r)
{
	if (r.ec != std::errc{})
		throw Exception{std::make_error_code(r.ec)};
	getSink().produced(reinterpret_cast<std::byte*>(r.ptr) - getSink().begin());
	return *this;
}

TextOutput&
TextOutput::operator<<(bool const b)
{ return reinterpret_cast<TextOutput&>(write(b ? "true" : "false", 5 - b)); }

std::size_t
Text::UppercaseHash::operator()(std::string const& h) const
{	// Convert to uppercase before calculating the hash value
	auto upper{h};
	for (auto& c : upper)
		c &= static_cast<char>(0b11011111);
	return std::hash<std::string>{}(upper);
}

bool
Text::CaseInsensitiveEqualTo::operator()(std::string const& a, std::string const& b) const noexcept
{	// Case-insensitive comparison
	std::string::size_type const sz{a.size()};
	if (b.size() != sz)
		return false;
	for (std::string::size_type i{0}; i < sz; ++i)
		if ((a[i] ^ b[i]) & static_cast<char>(0b11011111))
			return false;
	return true;
}

std::string_view
ToStringView(std::tuple<char const*, char const*, char const*> const& t)
{ return {std::get<0>(t), std::get<1>(t)}; }

std::string
ToString(std::tuple<char const*, char const*, char const*> const& t)
{ return {std::get<0>(t), std::get<1>(t)}; }

}//namespace Stream

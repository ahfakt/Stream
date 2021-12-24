#pragma once

#include "Stream/Buffer.hpp"
#include <charconv>
#include <limits>


namespace Stream {

class TextInput : public BufferReader {

	TextInput&
	checkFromChars(std::from_chars_result r);

protected:

	std::size_t
	provideFloat(std::size_t start, std::chars_format fmt, std::size_t intLimit, std::size_t fracLimit, std::size_t expLimit);

	std::size_t
	provideSignedInt(std::size_t start, std::size_t limit, unsigned base = 10);

	std::size_t
	provideUnsignedInt(std::size_t start, std::size_t limit, unsigned base = 10);

	std::size_t
	provideDecFrac(std::size_t start, std::size_t fracLimit);

	std::size_t
	provideDecExp(std::size_t start, std::size_t expLimit);

	std::size_t
	provideDigits10(std::size_t start, std::size_t limit, unsigned base = 10);

	std::size_t
	provideHexFrac(std::size_t start, std::size_t fracLimit);

	std::size_t
	provideHexExp(std::size_t start, std::size_t expLimit);

	std::size_t
	provideDigits36(std::size_t start, std::size_t limit, unsigned base = 16);

	std::pair<std::size_t, std::size_t const>
	provideUntil(std::size_t start, std::size_t limit, char delimiter = '\n');

public:

	TextInput() noexcept = default;

	TextInput(TextInput&& other) noexcept = default;

	TextInput&
	operator>>(bool& b) = delete; // TODO:

	TextInput&
	operator>>(Char auto& c);

	TextInput&
	operator>>(Integer auto& i);

	TextInput&
	fromChars(Integer auto& i, unsigned base);

	TextInput&
	operator>>(std::floating_point auto& f);

	TextInput&
	fromChars(std::floating_point auto& f, std::chars_format fmt, std::size_t precision = 0);

	std::string_view
	getLine(std::size_t limit = std::numeric_limits<std::size_t>::max());

	std::string_view
	getUntil(char delim = ' ', std::size_t limit = std::numeric_limits<std::size_t>::max());

	std::string_view
	getUntil(char const* regex = " ", std::size_t limit = std::numeric_limits<std::size_t>::max()) = delete; // TODO:

};//class Stream::TextInput


class TextOutput : public BufferWriter {

	TextOutput&
	checkToChars(std::to_chars_result r);

public:

	TextOutput() noexcept = default;

	TextOutput(TextOutput&& other) noexcept = default;

	TextOutput&
	operator<<(bool b);

	TextOutput&
	operator<<(Pointer auto p);

	TextOutput&
	operator<<(Char auto c);

	TextOutput&
	operator<<(Integer auto i);

	TextOutput&
	toChars(Integer auto i, unsigned base);

	TextOutput&
	operator<<(std::floating_point auto f);

	TextOutput&
	toChars(std::floating_point auto f, std::chars_format fmt);

	TextOutput&
	toChars(std::floating_point auto f, std::chars_format fmt, std::size_t precision);

	TextOutput&
	operator<<(Char auto const* s);

	template <Char C>
	TextOutput&
	operator<<(std::basic_string<C> const& s);

	template <Char C>
	TextOutput&
	operator<<(std::basic_string_view<C> const& s);

};//class Stream::TextOutput


class Text : public TextInput, public TextOutput {
public:

	struct UppercaseHash {
		std::size_t
		operator()(std::string const& h) const;
	};//struct Stream::Text::UppercaseHash

	struct CaseInsensitiveEqualTo {
		bool
		operator()(std::string const& a, std::string const& b) const noexcept;
	};//struct Stream::Text::CaseInsensitiveEqualTo

};//class Stream::Text


std::string_view
ToStringView(std::tuple<char const*, char const*, char const*> const& triplet);

std::string
ToString(std::tuple<char const*, char const*, char const*> const& triplet);

}//namespace Stream


#include "../../src/Text.tpp"

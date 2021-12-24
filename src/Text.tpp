#pragma once

#include "Stream/Text.hpp"

namespace Stream {

namespace detail {

consteval std::size_t digits10(int i)
{
	std::size_t r{1};
	while (i /= 10)
		++r;
	return r;
}

}//namespace Stream::detail

template <typename F>
constexpr std::size_t max_exponent10_digits10{detail::digits10(std::numeric_limits<F>::max_exponent10)};

template <typename F>
constexpr std::size_t max_exponent_digits10{detail::digits10(std::numeric_limits<F>::max_exponent)};

template <std::floating_point F>
constexpr std::size_t max_fixed_length{
	3 + // '-' '0' '.'
	std::numeric_limits<F>::max_digits10 + // precision
	std::numeric_limits<F>::max_exponent10
};

template <std::floating_point F>
constexpr std::size_t max_hex_length{
	3 + // '-' '1' '.'
	((std::numeric_limits<F>::digits + 1) >> 2) + // precision
	2 + // '-'/'+' 'p'/'P'
	max_exponent_digits10<F>
};

template <std::floating_point F>
constexpr std::size_t max_scientific_length{
	3 + // '-' 'd' '.'
	std::numeric_limits<F>::max_digits10 + // precision
	2 + // '-'/'+' 'e'/'E'
	max_exponent10_digits10<F>
};

TextInput&
TextInput::operator>>(Char auto& c)
{ return reinterpret_cast<TextInput&>(read(&c, sizeof c)); }

TextInput&
TextInput::operator>>(Integer auto& i)
{ return fromChars(i, 10); }

TextInput&
TextInput::fromChars(Integer auto& i, unsigned const base)
{
	using I = std::remove_reference_t<decltype(i)>;
	if (base < 2 || base > 36) [[unlikely]]
		throw Exception{std::make_error_code(std::errc::invalid_argument)};
	if (getSource().getDataSize() < std::numeric_limits<I>::digits) [[unlikely]] {
		if constexpr (SignedInteger<I>) {
			if (!provideSignedInt(0, std::numeric_limits<I>::digits + (base == 2), base))
				throw Exception{std::make_error_code(std::errc::invalid_argument)};
		} else {
			if (!provideUnsignedInt(0, std::numeric_limits<I>::digits, base))
				throw Exception{std::make_error_code(std::errc::invalid_argument)};
		}
	}
	return checkFromChars(std::from_chars(
		reinterpret_cast<char const*>(getSource().begin()),
		reinterpret_cast<char const*>(getSource().end()),
		i,
		base
	));
}

TextInput&
TextInput::operator>>(std::floating_point auto& f)
{ return fromChars(f, std::chars_format::general); }

TextInput&
TextInput::fromChars(std::floating_point auto& f, std::chars_format const fmt, std::size_t const precision)
{
	using F = std::remove_reference_t<decltype(f)>;
	std::size_t il{1}; // integer length is at least 1 digit
	std::size_t fl;
	std::size_t el;

	switch (fmt) {
		case std::chars_format::fixed: {
			// - 340282346638528859811704183484516925440
			// 0.000000000000000000000000000000000000011754944
			il += std::numeric_limits<F>::max_exponent10;
			fl = precision ? precision : std::numeric_limits<F>::max_digits10 + std::numeric_limits<F>::max_exponent10;
			el = 0;
		} break;
		case std::chars_format::scientific: {
			// -3.4028235e+38
			// 1.1754944e-38
			fl = precision ? precision : std::numeric_limits<F>::max_digits10;
			el = std::max(std::size_t{2}, max_exponent10_digits10<F>); // std::float16_t
		} break;
		case std::chars_format::general: {
			il += std::numeric_limits<F>::max_exponent10;
			fl = precision ? precision : std::numeric_limits<F>::max_digits10;
			el = std::max(std::size_t{2}, max_exponent10_digits10<F>);
		} break;
		case std::chars_format::hex: {
			// -1.fffffep+127
			// 1p-126
			fl = precision ? precision : (std::numeric_limits<F>::digits + 1) >> 2;
			el = max_exponent_digits10<F>;
		} break;
		default:
			throw Exception{std::make_error_code(std::errc::invalid_argument)};
	}

	if (getSource().getDataSize() < 1 + il + 1 + fl + 2 + el && !provideFloat(0, fmt, il, fl, el)) [[unlikely]]
		throw Exception{std::make_error_code(std::errc::invalid_argument)};

	return checkFromChars(std::from_chars(
		reinterpret_cast<char const*>(getSource().begin()),
		reinterpret_cast<char const*>(getSource().end()),
		f,
		fmt
	));
}

TextOutput&
TextOutput::operator<<(Pointer auto ptr)
{
	write("0x", 2);
	return toChars(reinterpret_cast<std::uintptr_t>(ptr), 16);
}

TextOutput&
TextOutput::operator<<(Char auto c)
{ return reinterpret_cast<TextOutput&>(write(&c, sizeof c)); }

TextOutput&
TextOutput::operator<<(Integer auto i)
{ return toChars(i, 10); }

TextOutput&
TextOutput::toChars(Integer auto i, unsigned const base)
{
	std::size_t size = CHAR_BIT * sizeof(i) + (base == 2);
	if (getSink().getSpaceSize() < size)
		getSink().allocSomeMore(size - getSink().getSpaceSize());
	return checkToChars(std::to_chars(
		reinterpret_cast<char*>(getSink().begin()),
		reinterpret_cast<char*>(const_cast<std::byte*>(getSink().end())),
		i,
		base
	));
}

TextOutput&
TextOutput::operator<<(std::floating_point auto f)
{
	using F = decltype(f);
	std::size_t size = std::min(max_fixed_length<F>, max_scientific_length<F>);
	if (getSink().getSpaceSize() < size)
		getSink().allocSomeMore(size - getSink().getSpaceSize());
	return checkToChars(std::to_chars(
		reinterpret_cast<char*>(getSink().begin()),
		reinterpret_cast<char*>(const_cast<std::byte*>(getSink().end())),
		f
	));
}

TextOutput&
TextOutput::toChars(std::floating_point auto f, std::chars_format const fmt)
{
	using F = decltype(f);
	std::size_t size = 
		fmt == std::chars_format::fixed
			? max_fixed_length<F>
			: (fmt == std::chars_format::hex
				? max_hex_length<F>
				: max_scientific_length<F>
			);
	if (getSink().getSpaceSize() < size)
		getSink().allocSomeMore(size - getSink().getSpaceSize());
	return checkToChars(std::to_chars(
		reinterpret_cast<char*>(getSink().begin()),
		reinterpret_cast<char*>(const_cast<std::byte*>(getSink().end())),
		f,
		fmt
	));
}

TextOutput&
TextOutput::toChars(std::floating_point auto f, std::chars_format const fmt, std::size_t const precision)
{
	using F = decltype(f);
	std::size_t size = 3 + precision +
		(fmt == std::chars_format::fixed
			? std::numeric_limits<F>::max_exponent10
			: 2 + (fmt == std::chars_format::hex
				? max_exponent_digits10<F>
				: max_exponent10_digits10<F>
			)
		);
	if (getSink().getSpaceSize() < size)
		getSink().allocSomeMore(size - getSink().getSpaceSize());
	return checkToChars(std::to_chars(
		reinterpret_cast<char*>(getSink().begin()),
		reinterpret_cast<char*>(const_cast<std::byte*>(getSink().end())),
		f,
		fmt,
		precision
	));
}

TextOutput&
TextOutput::operator<<(Char auto const* s)
{
	using C = std::remove_pointer_t<decltype(s)>;
	return reinterpret_cast<TextOutput&>(write(s, std::char_traits<C>::length(s) * sizeof(C)));
}

template <Char C>
TextOutput&
TextOutput::operator<<(std::basic_string<C> const& s)
{ return reinterpret_cast<TextOutput&>(write(s.data(), s.size() * sizeof(C))); }

template <Char C>
TextOutput&
TextOutput::operator<<(std::basic_string_view<C> const& s)
{ return reinterpret_cast<TextOutput&>(write(s.data(), s.size() * sizeof(C))); }

}//namespace Stream

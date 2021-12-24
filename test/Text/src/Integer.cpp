#include <Stream/Pipe.hpp>
#include <Stream/Text.hpp>
#include <cassert>

template <Stream::Integer I>
void
testInteger()
{
	Stream::Pipe pipe;
	Stream::Buffer buffer(pipe.getBufferSize().value());
	Stream::Text text;
	pipe | buffer | text;

	I min_{1}, min{std::numeric_limits<I>::min()};
	I max_{1}, max{std::numeric_limits<I>::max()};
	char c;

	for (int i = 2; i < 37; ++i) {
		text.toChars(min, i) << '\n';
		text.toChars(max, i) << '\n';
	}
	text < nullptr;

	for (int i = 2; i < 37; ++i) {
		text.fromChars(min_, i) >> c;
		assert(min_ == min);

		text.fromChars(max_, i) >> c;
		assert(max_ == max);
	}
}

int main()
{
	testInteger<signed char>();
	testInteger<short>();
	testInteger<short int>();
	testInteger<signed short>();
	testInteger<signed short int>();
	testInteger<int>();
	testInteger<signed>();
	testInteger<signed int>();
	testInteger<long>();
	testInteger<long int>();
	testInteger<signed long>();
	testInteger<signed long int>();
	testInteger<long long>();
	testInteger<long long int>();
	testInteger<signed long long>();
	testInteger<signed long long int>();
	testInteger<std::int8_t>();
	testInteger<std::int16_t>();
	testInteger<std::int32_t>();
	testInteger<std::int64_t>();
	testInteger<std::int_fast8_t>();
	testInteger<std::int_fast16_t>();
	testInteger<std::int_fast32_t>();
	testInteger<std::int_fast64_t>();
	testInteger<std::int_least8_t>();
	testInteger<std::int_least16_t>();
	testInteger<std::int_least32_t>();
	testInteger<std::int_least64_t>();
	testInteger<std::intmax_t>();
	testInteger<std::intptr_t>();

	testInteger<unsigned char>();
	testInteger<unsigned short>();
	testInteger<unsigned short int>();
	testInteger<unsigned>();
	testInteger<unsigned int>();
	testInteger<unsigned long>();
	testInteger<unsigned long int>();
	testInteger<unsigned long long>();
	testInteger<unsigned long long int>();
	testInteger<std::uint8_t>();
	testInteger<std::uint16_t>();
	testInteger<std::uint32_t>();
	testInteger<std::uint64_t>();
	testInteger<std::uint_fast8_t>();
	testInteger<std::uint_fast16_t>();
	testInteger<std::uint_fast32_t>();
	testInteger<std::uint_fast64_t>();
	testInteger<std::uint_least8_t>();
	testInteger<std::uint_least16_t>();
	testInteger<std::uint_least32_t>();
	testInteger<std::uint_least64_t>();
	testInteger<std::uintmax_t>();
	testInteger<std::uintptr_t>();
	testInteger<std::size_t>();

	return 0;
}


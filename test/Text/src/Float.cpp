#include <Stream/Pipe.hpp>
#include <Stream/Text.hpp>
#include <cassert>
#include <iostream>
#include <iomanip>

#if __cplusplus > 202002L
#include <stdfloat>
#endif // C++23

template <std::floating_point F>
void
testFloatingPoint(std::chars_format format, std::size_t prec = 0)
{
	Stream::Pipe pipe;
	Stream::Buffer buffer(pipe.getBufferSize().value());
	Stream::Text text;
	pipe | buffer | text;

	F low_{0}, low{std::numeric_limits<F>::lowest()};
	F min_{0}, min{std::numeric_limits<F>::min()};
	char c;

	if (!prec) {
		text.toChars(low, format) << '\n';
		text.toChars(min, format) << '\n';
	} else {
		text.toChars(low, format, prec) << '\n';
		text.toChars(min, format, prec) << '\n';
	}
	text < nullptr;

	text.fromChars(low_, format, prec) >> c;
	assert(low_ == low);

	text.fromChars(min_, format, prec) >> c;
	assert(min_ == min);
}


template <std::floating_point F>
void
testFloatingPoint()
{
	std::cout
		<< "|----------------|" << "----------------|" << "----------------|" << "----------------|" << "----------------|" << "----------------|" << "----------------|" << "----------------|" << '\n'
		<< "| " << std::setw(14) << typeid(F{}).name() << " |"
		<< std::setw(15) << std::numeric_limits<F>::digits << " |"
		<< std::setw(15) << std::numeric_limits<F>::digits10 << " |"
		<< std::setw(15) << std::numeric_limits<F>::max_digits10 << " |"
		<< std::setw(15) << std::numeric_limits<F>::min_exponent << " |"
		<< std::setw(15) << std::numeric_limits<F>::max_exponent << " |"
		<< std::setw(15) << std::numeric_limits<F>::min_exponent10 << " |"
		<< std::setw(15) << std::numeric_limits<F>::max_exponent10 << " |"
		<< '\n';
	testFloatingPoint<F>(std::chars_format::fixed);
	testFloatingPoint<F>(std::chars_format::scientific);
	testFloatingPoint<F>(std::chars_format::general);
	testFloatingPoint<F>(std::chars_format::hex);

	testFloatingPoint<F>(std::chars_format::fixed, 5 + std::numeric_limits<F>::digits10 + 1 + std::numeric_limits<F>::max_exponent10);
	testFloatingPoint<F>(std::chars_format::scientific, 5 + std::numeric_limits<F>::digits10 + 1);
	testFloatingPoint<F>(std::chars_format::general, 5 + std::numeric_limits<F>::digits10 + 1);
	testFloatingPoint<F>(std::chars_format::hex, 5 + ((std::numeric_limits<F>::digits + 1) >> 2));
}

int main()
{
	std::cout
		<< "|-----------------" << "-----------------" << "-----------------" << "-----------------" << "-----------------" << "-----------------" << "-----------------" << "-----------------" << '\n'
		<< "|            |" << " digits         |" << " digits10       |" << " max_digits10   |" << " min_exponent   |" << " max_exponent   |" << " min_exponent10 |" << " max_exponent10 |" << '\n';

	testFloatingPoint<float>();
	testFloatingPoint<double>();
	testFloatingPoint<long double>();

#if __cplusplus > 202002L
	testFloatingPoint<std::float16_t>();
	testFloatingPoint<std::float32_t>();
	testFloatingPoint<std::float64_t>();
	testFloatingPoint<std::float128_t>();
	testFloatingPoint<std::bfloat16_t>();
#endif // C++23

	return 0;
}


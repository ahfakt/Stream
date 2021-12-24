#include <Stream/InOut.hpp>
#if __cplusplus > 202002L
#include <stdfloat>
#endif // C++23


static_assert(!Stream::UnsignedInteger<signed char>				);
static_assert(!Stream::UnsignedInteger<short>					);
static_assert(!Stream::UnsignedInteger<short int>				);
static_assert(!Stream::UnsignedInteger<signed short>			);
static_assert(!Stream::UnsignedInteger<signed short int>		);
static_assert(!Stream::UnsignedInteger<int>						);
static_assert(!Stream::UnsignedInteger<signed>					);
static_assert(!Stream::UnsignedInteger<signed int>				);
static_assert(!Stream::UnsignedInteger<long>					);
static_assert(!Stream::UnsignedInteger<long int>				);
static_assert(!Stream::UnsignedInteger<signed long>				);
static_assert(!Stream::UnsignedInteger<signed long int>			);
static_assert(!Stream::UnsignedInteger<long long>				);
static_assert(!Stream::UnsignedInteger<long long int>			);
static_assert(!Stream::UnsignedInteger<signed long long>		);
static_assert(!Stream::UnsignedInteger<signed long long int>	);
static_assert(!Stream::UnsignedInteger<std::int8_t>				);
static_assert(!Stream::UnsignedInteger<std::int16_t>			);
static_assert(!Stream::UnsignedInteger<std::int32_t>			);
static_assert(!Stream::UnsignedInteger<std::int64_t>			);
static_assert(!Stream::UnsignedInteger<std::int_fast8_t>		);
static_assert(!Stream::UnsignedInteger<std::int_fast16_t>		);
static_assert(!Stream::UnsignedInteger<std::int_fast32_t>		);
static_assert(!Stream::UnsignedInteger<std::int_fast64_t>		);
static_assert(!Stream::UnsignedInteger<std::int_least8_t>		);
static_assert(!Stream::UnsignedInteger<std::int_least16_t>		);
static_assert(!Stream::UnsignedInteger<std::int_least32_t>		);
static_assert(!Stream::UnsignedInteger<std::int_least64_t>		);
static_assert(!Stream::UnsignedInteger<std::intmax_t>			);
static_assert(!Stream::UnsignedInteger<std::intptr_t>			);

static_assert( Stream::UnsignedInteger<unsigned char>			);
static_assert( Stream::UnsignedInteger<unsigned short>			);
static_assert( Stream::UnsignedInteger<unsigned short int>		);
static_assert( Stream::UnsignedInteger<unsigned>				);
static_assert( Stream::UnsignedInteger<unsigned int>			);
static_assert( Stream::UnsignedInteger<unsigned long>			);
static_assert( Stream::UnsignedInteger<unsigned long int>		);
static_assert( Stream::UnsignedInteger<unsigned long long>		);
static_assert( Stream::UnsignedInteger<unsigned long long int>	);
static_assert( Stream::UnsignedInteger<std::uint8_t>			);
static_assert( Stream::UnsignedInteger<std::uint16_t>			);
static_assert( Stream::UnsignedInteger<std::uint32_t>			);
static_assert( Stream::UnsignedInteger<std::uint64_t>			);
static_assert( Stream::UnsignedInteger<std::uint_fast8_t>		);
static_assert( Stream::UnsignedInteger<std::uint_fast16_t>		);
static_assert( Stream::UnsignedInteger<std::uint_fast32_t>		);
static_assert( Stream::UnsignedInteger<std::uint_fast64_t>		);
static_assert( Stream::UnsignedInteger<std::uint_least8_t>		);
static_assert( Stream::UnsignedInteger<std::uint_least16_t>		);
static_assert( Stream::UnsignedInteger<std::uint_least32_t>		);
static_assert( Stream::UnsignedInteger<std::uint_least64_t>		);
static_assert( Stream::UnsignedInteger<std::uintmax_t>			);
static_assert( Stream::UnsignedInteger<std::uintptr_t>			);
static_assert( Stream::UnsignedInteger<std::size_t>				);

static_assert(!Stream::UnsignedInteger<bool>					);

static_assert(!Stream::UnsignedInteger<char>					);
static_assert(!Stream::UnsignedInteger<wchar_t>					);
static_assert(!Stream::UnsignedInteger<char8_t>					);
static_assert(!Stream::UnsignedInteger<char16_t>				);
static_assert(!Stream::UnsignedInteger<char32_t>				);

static_assert(!Stream::UnsignedInteger<float>					);
static_assert(!Stream::UnsignedInteger<double>					);
static_assert(!Stream::UnsignedInteger<long double>				);
#if __cplusplus > 202002L
static_assert(!Stream::UnsignedInteger<std::float16_t>			);
static_assert(!Stream::UnsignedInteger<std::float32_t>			);
static_assert(!Stream::UnsignedInteger<std::float64_t>			);
static_assert(!Stream::UnsignedInteger<std::float128_t>			);
static_assert(!Stream::UnsignedInteger<std::bfloat16_t>			);
#endif // C++23


int main()
{ return 0; }

#include <Stream/InOut.hpp>
#if __cplusplus > 202002L
#include <stdfloat>
#endif // C++23


static_assert(!Stream::Char<signed char>			);
static_assert(!Stream::Char<short>					);
static_assert(!Stream::Char<short int>				);
static_assert(!Stream::Char<signed short>			);
static_assert(!Stream::Char<signed short int>		);
static_assert(!Stream::Char<int>					);
static_assert(!Stream::Char<signed>					);
static_assert(!Stream::Char<signed int>				);
static_assert(!Stream::Char<long>					);
static_assert(!Stream::Char<long int>				);
static_assert(!Stream::Char<signed long>			);
static_assert(!Stream::Char<signed long int>		);
static_assert(!Stream::Char<long long>				);
static_assert(!Stream::Char<long long int>			);
static_assert(!Stream::Char<signed long long>		);
static_assert(!Stream::Char<signed long long int>	);
static_assert(!Stream::Char<std::int8_t>			);
static_assert(!Stream::Char<std::int16_t>			);
static_assert(!Stream::Char<std::int32_t>			);
static_assert(!Stream::Char<std::int64_t>			);
static_assert(!Stream::Char<std::int_fast8_t>		);
static_assert(!Stream::Char<std::int_fast16_t>		);
static_assert(!Stream::Char<std::int_fast32_t>		);
static_assert(!Stream::Char<std::int_fast64_t>		);
static_assert(!Stream::Char<std::int_least8_t>		);
static_assert(!Stream::Char<std::int_least16_t>		);
static_assert(!Stream::Char<std::int_least32_t>		);
static_assert(!Stream::Char<std::int_least64_t>		);
static_assert(!Stream::Char<std::intmax_t>			);
static_assert(!Stream::Char<std::intptr_t>			);

static_assert(!Stream::Char<unsigned char>			);
static_assert(!Stream::Char<unsigned short>			);
static_assert(!Stream::Char<unsigned short int>		);
static_assert(!Stream::Char<unsigned>				);
static_assert(!Stream::Char<unsigned int>			);
static_assert(!Stream::Char<unsigned long>			);
static_assert(!Stream::Char<unsigned long int>		);
static_assert(!Stream::Char<unsigned long long>		);
static_assert(!Stream::Char<unsigned long long int>	);
static_assert(!Stream::Char<std::uint8_t>			);
static_assert(!Stream::Char<std::uint16_t>			);
static_assert(!Stream::Char<std::uint32_t>			);
static_assert(!Stream::Char<std::uint64_t>			);
static_assert(!Stream::Char<std::uint_fast8_t>		);
static_assert(!Stream::Char<std::uint_fast16_t>		);
static_assert(!Stream::Char<std::uint_fast32_t>		);
static_assert(!Stream::Char<std::uint_fast64_t>		);
static_assert(!Stream::Char<std::uint_least8_t>		);
static_assert(!Stream::Char<std::uint_least16_t>	);
static_assert(!Stream::Char<std::uint_least32_t>	);
static_assert(!Stream::Char<std::uint_least64_t>	);
static_assert(!Stream::Char<std::uintmax_t>			);
static_assert(!Stream::Char<std::uintptr_t>			);
static_assert(!Stream::Char<std::size_t>			);

static_assert(!Stream::Char<bool>					);

static_assert( Stream::Char<char>					);
static_assert( Stream::Char<wchar_t>				);
static_assert( Stream::Char<char8_t>				);
static_assert( Stream::Char<char16_t>				);
static_assert( Stream::Char<char32_t>				);

static_assert(!Stream::Char<float>					);
static_assert(!Stream::Char<double>					);
static_assert(!Stream::Char<long double>			);
#if __cplusplus > 202002L
static_assert(!Stream::Char<std::float16_t>			);
static_assert(!Stream::Char<std::float32_t>			);
static_assert(!Stream::Char<std::float64_t>			);
static_assert(!Stream::Char<std::float128_t>		);
static_assert(!Stream::Char<std::bfloat16_t>		);
#endif // C++23


int main()
{ return 0; }

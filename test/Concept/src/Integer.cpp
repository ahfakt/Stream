#include <Stream/InOut.hpp>
#if __cplusplus > 202002L
#include <stdfloat>
#endif // C++23


static_assert( Stream::Integer<signed char>				);
static_assert( Stream::Integer<short>					);
static_assert( Stream::Integer<short int>				);
static_assert( Stream::Integer<signed short>			);
static_assert( Stream::Integer<signed short int>		);
static_assert( Stream::Integer<int>						);
static_assert( Stream::Integer<signed>					);
static_assert( Stream::Integer<signed int>				);
static_assert( Stream::Integer<long>					);
static_assert( Stream::Integer<long int>				);
static_assert( Stream::Integer<signed long>				);
static_assert( Stream::Integer<signed long int>			);
static_assert( Stream::Integer<long long>				);
static_assert( Stream::Integer<long long int>			);
static_assert( Stream::Integer<signed long long>		);
static_assert( Stream::Integer<signed long long int>	);
static_assert( Stream::Integer<std::int8_t>				);
static_assert( Stream::Integer<std::int16_t>			);
static_assert( Stream::Integer<std::int32_t>			);
static_assert( Stream::Integer<std::int64_t>			);
static_assert( Stream::Integer<std::int_fast8_t>		);
static_assert( Stream::Integer<std::int_fast16_t>		);
static_assert( Stream::Integer<std::int_fast32_t>		);
static_assert( Stream::Integer<std::int_fast64_t>		);
static_assert( Stream::Integer<std::int_least8_t>		);
static_assert( Stream::Integer<std::int_least16_t>		);
static_assert( Stream::Integer<std::int_least32_t>		);
static_assert( Stream::Integer<std::int_least64_t>		);
static_assert( Stream::Integer<std::intmax_t>			);
static_assert( Stream::Integer<std::intptr_t>			);

static_assert( Stream::Integer<unsigned char>			);
static_assert( Stream::Integer<unsigned short>			);
static_assert( Stream::Integer<unsigned short int>		);
static_assert( Stream::Integer<unsigned>				);
static_assert( Stream::Integer<unsigned int>			);
static_assert( Stream::Integer<unsigned long>			);
static_assert( Stream::Integer<unsigned long int>		);
static_assert( Stream::Integer<unsigned long long>		);
static_assert( Stream::Integer<unsigned long long int>	);
static_assert( Stream::Integer<std::uint8_t>			);
static_assert( Stream::Integer<std::uint16_t>			);
static_assert( Stream::Integer<std::uint32_t>			);
static_assert( Stream::Integer<std::uint64_t>			);
static_assert( Stream::Integer<std::uint_fast8_t>		);
static_assert( Stream::Integer<std::uint_fast16_t>		);
static_assert( Stream::Integer<std::uint_fast32_t>		);
static_assert( Stream::Integer<std::uint_fast64_t>		);
static_assert( Stream::Integer<std::uint_least8_t>		);
static_assert( Stream::Integer<std::uint_least16_t>		);
static_assert( Stream::Integer<std::uint_least32_t>		);
static_assert( Stream::Integer<std::uint_least64_t>		);
static_assert( Stream::Integer<std::uintmax_t>			);
static_assert( Stream::Integer<std::uintptr_t>			);
static_assert( Stream::Integer<std::size_t>				);

static_assert( Stream::Integer<bool>					);

static_assert(!Stream::Integer<char>					);
static_assert(!Stream::Integer<wchar_t>					);
static_assert(!Stream::Integer<char8_t>					);
static_assert(!Stream::Integer<char16_t>				);
static_assert(!Stream::Integer<char32_t>				);

static_assert(!Stream::Integer<float>					);
static_assert(!Stream::Integer<double>					);
static_assert(!Stream::Integer<long double>				);
#if __cplusplus > 202002L
static_assert(!Stream::Integer<std::float16_t>			);
static_assert(!Stream::Integer<std::float32_t>			);
static_assert(!Stream::Integer<std::float64_t>			);
static_assert(!Stream::Integer<std::float128_t>			);
static_assert(!Stream::Integer<std::bfloat16_t>			);
#endif // C++23


int main()
{ return 0; }

#include <Stream/InOut.hpp>
#if __cplusplus > 202002L
#include <stdfloat>
#endif // C++23


static_assert( Stream::SignedInteger<signed char>			);
static_assert( Stream::SignedInteger<short>					);
static_assert( Stream::SignedInteger<short int>				);
static_assert( Stream::SignedInteger<signed short>			);
static_assert( Stream::SignedInteger<signed short int>		);
static_assert( Stream::SignedInteger<int>					);
static_assert( Stream::SignedInteger<signed>				);
static_assert( Stream::SignedInteger<signed int>			);
static_assert( Stream::SignedInteger<long>					);
static_assert( Stream::SignedInteger<long int>				);
static_assert( Stream::SignedInteger<signed long>			);
static_assert( Stream::SignedInteger<signed long int>		);
static_assert( Stream::SignedInteger<long long>				);
static_assert( Stream::SignedInteger<long long int>			);
static_assert( Stream::SignedInteger<signed long long>		);
static_assert( Stream::SignedInteger<signed long long int>	);
static_assert( Stream::SignedInteger<std::int8_t>			);
static_assert( Stream::SignedInteger<std::int16_t>			);
static_assert( Stream::SignedInteger<std::int32_t>			);
static_assert( Stream::SignedInteger<std::int64_t>			);
static_assert( Stream::SignedInteger<std::int_fast8_t>		);
static_assert( Stream::SignedInteger<std::int_fast16_t>		);
static_assert( Stream::SignedInteger<std::int_fast32_t>		);
static_assert( Stream::SignedInteger<std::int_fast64_t>		);
static_assert( Stream::SignedInteger<std::int_least8_t>		);
static_assert( Stream::SignedInteger<std::int_least16_t>	);
static_assert( Stream::SignedInteger<std::int_least32_t>	);
static_assert( Stream::SignedInteger<std::int_least64_t>	);
static_assert( Stream::SignedInteger<std::intmax_t>			);
static_assert( Stream::SignedInteger<std::intptr_t>			);

static_assert(!Stream::SignedInteger<unsigned char>			);
static_assert(!Stream::SignedInteger<unsigned short>		);
static_assert(!Stream::SignedInteger<unsigned short int>	);
static_assert(!Stream::SignedInteger<unsigned>				);
static_assert(!Stream::SignedInteger<unsigned int>			);
static_assert(!Stream::SignedInteger<unsigned long>			);
static_assert(!Stream::SignedInteger<unsigned long int>		);
static_assert(!Stream::SignedInteger<unsigned long long>	);
static_assert(!Stream::SignedInteger<unsigned long long int>);
static_assert(!Stream::SignedInteger<std::uint8_t>			);
static_assert(!Stream::SignedInteger<std::uint16_t>			);
static_assert(!Stream::SignedInteger<std::uint32_t>			);
static_assert(!Stream::SignedInteger<std::uint64_t>			);
static_assert(!Stream::SignedInteger<std::uint_fast8_t>		);
static_assert(!Stream::SignedInteger<std::uint_fast16_t>	);
static_assert(!Stream::SignedInteger<std::uint_fast32_t>	);
static_assert(!Stream::SignedInteger<std::uint_fast64_t>	);
static_assert(!Stream::SignedInteger<std::uint_least8_t>	);
static_assert(!Stream::SignedInteger<std::uint_least16_t>	);
static_assert(!Stream::SignedInteger<std::uint_least32_t>	);
static_assert(!Stream::SignedInteger<std::uint_least64_t>	);
static_assert(!Stream::SignedInteger<std::uintmax_t>		);
static_assert(!Stream::SignedInteger<std::uintptr_t>		);
static_assert(!Stream::SignedInteger<std::size_t>			);

static_assert(!Stream::SignedInteger<bool>					);

static_assert(!Stream::SignedInteger<char>					);
static_assert(!Stream::SignedInteger<wchar_t>				);
static_assert(!Stream::SignedInteger<char8_t>				);
static_assert(!Stream::SignedInteger<char16_t>				);
static_assert(!Stream::SignedInteger<char32_t>				);

static_assert(!Stream::SignedInteger<float>					);
static_assert(!Stream::SignedInteger<double>				);
static_assert(!Stream::SignedInteger<long double>			);
#if __cplusplus > 202002L
static_assert(!Stream::SignedInteger<std::float16_t>		);
static_assert(!Stream::SignedInteger<std::float32_t>		);
static_assert(!Stream::SignedInteger<std::float64_t>		);
static_assert(!Stream::SignedInteger<std::float128_t>		);
static_assert(!Stream::SignedInteger<std::bfloat16_t>		);
#endif // C++23


int main()
{ return 0; }

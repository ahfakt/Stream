#include <Stream/InOut.hpp>

using namespace Stream;

// public and unambiguous Input base
// https://en.cppreference.com/w/cpp/concepts/derived_from
static_assert(!std::derived_from<int, Input> && !Source<int>);
static_assert(std::derived_from<Input, Input> && Source<Input>);

class PrivateInput : Input {};
static_assert(!std::derived_from<PrivateInput, Input> && !Source<PrivateInput>);

struct PublicInput : Input {};
static_assert(std::derived_from<PublicInput, Input> && Source<PublicInput>);


int main()
{
	return 0;
}

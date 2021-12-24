#include <Stream/InOut.hpp>

using namespace Stream;

struct PublicInput : Input {};
struct E
{
	friend PublicInput& operator>>(Input& input, E&);
};
static_assert(Source<PublicInput> && HasExtraction<E, Input&>);


// There is a operator>> template for trivially copyable types 
// https://en.cppreference.com/w/cpp/types/is_trivially_copyable
struct A { int m; };
static_assert(std::is_trivially_copyable_v<A> && HasExtraction<A, Input&>);

struct B { B(B const&) {} };
static_assert(!std::is_trivially_copyable_v<B> && !HasExtraction<B, Input&>);
 
struct C { virtual void foo(); };
static_assert(!std::is_trivially_copyable_v<C> && !HasExtraction<C, Input&>);
 
struct D
{
    int m;
 
    D(D const&) = default; // -> trivially copyable

    explicit
    D(int x) : m(x + 1) {}
};
static_assert(std::is_trivially_copyable_v<D> && HasExtraction<D, Input&>);


int main()
{
	return 0;
}

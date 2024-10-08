#include "Stream/InOut.hpp"

namespace Stream {

template <typename In>
concept Source =
	std::derived_from<In, Input>;

template <typename InputRef>
concept SourceRef =
	std::is_lvalue_reference_v<InputRef> &&
	Source<std::remove_reference_t<InputRef>>;

template <typename T, typename InputRef>
concept HasExtraction =
	requires { std::declval<InputRef>() >> std::declval<T&>(); } &&
	SourceRef<decltype(std::declval<InputRef>() >> std::declval<T&>())>;

template <typename T, typename ... Args>
concept Initializable =
	requires { T{std::declval<Args>() ...}; } &&
	std::is_nothrow_destructible_v<T>;

template <typename T, typename InputRef, typename ... Args>
concept Extractable =
	SourceRef<InputRef> &&
	!Initializable<T, InputRef, Args ...> &&
	((sizeof...(Args) == 0 && std::is_trivially_default_constructible_v<T>) || Initializable<T, Args ...>) &&
	HasExtraction<T, InputRef>;

template <typename T, typename InputRef, typename ... Args>
concept Deserializable =
	SourceRef<InputRef> && (
		Initializable<T, InputRef, Args ...> || (
			((sizeof...(Args) == 0 && std::is_trivially_default_constructible_v<T>) || Initializable<T, Args ...>) &&
			HasExtraction<T, InputRef>
		)
	);

template <typename Out>
concept Sink =
	std::derived_from<Out, Output>;

template <typename OutputRef>
concept SinkRef =
	std::is_lvalue_reference_v<OutputRef> &&
	Sink<std::remove_reference_t<OutputRef>>;

template <typename T, typename OutputRef>
concept HasInsertion =
	requires { std::declval<OutputRef>() << std::declval<T const&>(); } &&
	SinkRef<decltype(std::declval<OutputRef>() << std::declval<T const&>())>;

template <typename T, typename OutputRef>
concept InsertableTo =
	SinkRef<OutputRef> &&
	HasInsertion<T, OutputRef>;

Input&
Input::operator>>(auto& t)
requires std::is_trivially_copyable_v<std::remove_reference_t<decltype(t)>>
{ return read(reinterpret_cast<void*>(&t), sizeof t); }

template <Char C>
Input&
Input::operator>>(std::basic_string<C>& s)
{
	std::uint64_t size{0};
	read(&size, sizeof size);
	s.resize(size);
	return read(s.data(), size * sizeof(C));
}

Input&
Input::operator>>(Char auto* s)
{
	using C = std::remove_pointer_t<decltype(s)>;
	std::uint64_t size{0};
	read(&size, sizeof size);
	return read(s, size * sizeof(C));
}

/**
 * @brief	Initialize a T object with input and optional additional args
 * @tparam	T
 * @param	input
 * @param	args
 * @return	T object
 * @details
 */
template <typename T>
T
Get(Source auto& input, auto&& ... args)
requires Initializable<T, decltype(input), decltype(args) ...>
{ return T{input, std::forward<decltype(args)>(args) ...}; }

/**
 * @brief	Initialize a T object with optional args and extract it from input
 * @tparam	T
 * @param	input
 * @param	args
 * @return	T object
 * @details
 */
template <typename T>
T
Get(Source auto& input, auto&& ... args)
requires Extractable<T, decltype(input), decltype(args) ...>
{
	if constexpr (sizeof...(args) == 0 && std::is_trivially_default_constructible_v<T>) {
		T t;
		input >> t;
		return t;
	} else {
		T t{std::forward<decltype(args)>(args) ...};
		input >> t;
		return t;
	}
}

Output&
Output::operator<<(auto const& t)
requires std::is_trivially_copyable_v<std::remove_reference_t<decltype(t)>>
{ return write(reinterpret_cast<void const*>(&t), sizeof t); }

template <Char C>
Output&
Output::operator<<(std::basic_string<C> const& s)
{
	std::uint64_t size{s.size()};
	return write(&size, sizeof size).write(s.data(), size * sizeof(C));
}

Output&
Output::operator<<(Char auto const* s)
{
	using C = std::remove_pointer_t<decltype(s)>;
	std::uint64_t size{std::char_traits<C>::length(s)};
	return write(&size, sizeof size).write(s, size * sizeof(C));
}

}//namespace Stream

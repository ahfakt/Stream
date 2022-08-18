#include "Stream/InOut.hpp"

namespace Stream {

template <typename InputRef, typename T>
concept ExtractableTo =
	std::derived_from<std::remove_reference_t<InputRef>, Input> &&
	std::derived_from<std::remove_reference_t<InputRef>, std::remove_reference_t<decltype(std::declval<InputRef>() >> std::declval<T&>())>>;

template <typename InputRef, typename T>
concept TriviallyExtractableTo =
	std::is_trivially_default_constructible_v<T> &&
	ExtractableTo<InputRef, T>;

template <typename T, typename ... Args>
concept ConstructibleFrom = std::destructible<T> && requires
{ T{std::declval<Args>() ...}; };

template <typename T, typename ... Args>
concept NotConstructibleFrom = !ConstructibleFrom<T, Args ...>;

template <typename T, typename InputRef, typename ... Args>
concept DeserializableWith = std::derived_from<std::remove_reference_t<InputRef>, Stream::Input> &&
	(ConstructibleFrom<T, InputRef, Args ...> ||
	(ConstructibleFrom<T, Args ...> || std::is_trivially_default_constructible_v<T>) &&
		std::derived_from<std::remove_reference_t<InputRef>, std::remove_reference_t<decltype(std::declval<InputRef>() >> std::declval<T&>())>>);

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
 * @brief	Construct a T object with input and optional additional args
 * @tparam	T
 * @param	input
 * @param	args
 * @return
 * @details
 */
template <typename T>
T
Get(std::derived_from<Input> auto& input, auto&& ... args)
{ return {input, std::forward<decltype(args)>(args) ...}; }

/**
 * @brief	Construct a T object with optional args and extract it from input
 * @tparam	T
 * @param	input
 * @param	args
 * @return
 * @details
 */
template <typename T>
T
Get(ExtractableTo<T> auto& input, auto&& ... args)
requires NotConstructibleFrom<T, decltype(input), decltype(args) ...>
{
	T t{std::forward<decltype(args)>(args) ...};
	input >> t;
	return t;
}

/**
 * @brief	Trivially default construct a T object and extract it from input
 * @tparam	T
 * @param	input
 * @return
 * @details
 */
template <typename T>
T
Get(TriviallyExtractableTo<T> auto& input)
requires NotConstructibleFrom<T, decltype(input)>
{
	T t;
	input >> t;
	return t;
}

template <typename T, typename OutputRef>
concept InsertableTo =
std::derived_from<std::remove_reference_t<OutputRef>, Output> &&
std::derived_from<std::remove_reference_t<OutputRef>, std::remove_reference_t<decltype(std::declval<OutputRef>() << std::declval<T const&>())>>;

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

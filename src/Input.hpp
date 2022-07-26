#include "Stream/Input.h"

namespace std {

template <>
struct is_error_code_enum<Stream::Input::Exception::Code> : true_type {};

}//namespace std

namespace Stream {

template <typename InputType, typename T>
concept ExtractableTo =
	std::derived_from<InputType, Input> &&
	std::derived_from<InputType, std::remove_reference_t<decltype(std::declval<InputType&>() >> std::declval<T&>())>>;

template <typename InputType, typename T>
concept TriviallyExtractableTo =
	std::is_trivially_default_constructible_v<T> &&
	ExtractableTo<InputType, T>;

template <typename T, typename ... Args>
concept ConstructibleFrom = std::destructible<T> && requires
{ T{std::declval<Args>() ...}; };

template <typename T, typename InputType, typename ... Args>
concept DeserializableWith = std::derived_from<InputType, Stream::Input> &&
	(ConstructibleFrom<T, InputType&, Args ...> ||
	(ConstructibleFrom<T, Args ...> || std::is_trivially_default_constructible_v<T>) &&
		std::derived_from<InputType, std::remove_reference_t<decltype(std::declval<InputType&>() >> std::declval<T&>())>>);

Input&
Input::operator>>(auto& t)
requires std::is_trivially_copyable_v<std::remove_reference_t<decltype(t)>>
{ return read(reinterpret_cast<void*>(&t), sizeof t); }

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
requires (!ConstructibleFrom<T, decltype(input), decltype(args) ...>)
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
requires (!ConstructibleFrom<T, decltype(input)>)
{
	T t;
	input >> t;
	return t;
}

}//namespace Stream

#include "Stream/Input.h"

namespace std {

template <>
struct is_error_code_enum<Stream::Input::Exception::Code> : true_type {};

}//namespace std

namespace Stream {

template <typename T>
concept In = std::is_base_of_v<Input, T>;

template <typename T>
concept InFilter = std::is_base_of_v<InputFilter, T>;

Input&
Input::operator>>(auto& t)
requires std::is_trivially_copyable_v<std::remove_cvref_t<decltype(t)>>
{ return read(reinterpret_cast<void*>(&t), sizeof(t)); }

/**
 * @brief	Deserializable concept
 * @tparam	T
 * @tparam	InputRef Derived Input stream type
 * @details	Requires T has an <b>InputRef& operator>>(InputRef&, T&)</b> overload
 */
template <typename T, typename InputRef>
concept Deserializable = In<std::remove_cvref_t<InputRef>> && requires(InputRef is, T& t)
{{ is >> t } -> std::same_as<InputRef>; };

/**
 * @brief	Constructible concept
 * @tparam	T
 * @tparam	InputRef Derived Input stream type
 * @tparam	Args Optional additional parameter types to be used to construct T
 * @details	Requires T has a <b>T(InputRef&, Args ...)</b> constructor
 */
template <typename T, typename InputRef, typename ... Args>
concept Constructible = In<std::remove_cvref_t<InputRef>> && std::constructible_from<T, InputRef, Args ...>;

/**
 * @brief	Extractable concept
 * @tparam	T
 * @tparam	Args Optional parameter types to be used to construct T
 * @details	Requires T has either trivial default constructor or a <b>T(Args ...)</b> constructor
 * 			and <b>In& operator>>(In&, T&)</b> overload
 */
template <typename T, typename InputType, typename ... Args>
concept Extractable = Deserializable<T, InputType> && (std::is_trivially_default_constructible_v<T> || std::constructible_from<T, Args ...>);

template <typename T, typename ... Args>
T
Get(In auto& input, Args&& ... args)
requires std::constructible_from<T, decltype(input), Args ...>
{ return T(input, std::forward<Args>(args) ...); }

template <typename T, typename ... Args>
T
Get(In auto& input, Args&& ... args)
requires std::constructible_from<T, Args ...> && Deserializable<T, decltype(input)>
{
	T t(std::forward<Args>(args) ...);
	input >> t;
	return t;
}

template <typename T>
T
Get(In auto& input)
requires std::is_trivially_default_constructible_v<T> && Deserializable<T, decltype(input)>
{
	T t;
	input >> t;
	return t;
}

}//namespace Stream

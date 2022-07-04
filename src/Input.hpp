#include "Stream/Input.h"

namespace Stream {

auto&
operator>>(In auto& input, auto& t)
requires std::is_trivially_copyable_v<decltype(t)>
{
	input.read(reinterpret_cast<void*>(&t), sizeof(t));
	return input;
}

auto&
operator>>(In auto& input, std::string& str)
{
	str.clear();
	char c;
	input.read(&c, 1);
	while (c) {
		str.push_back(c);
		input.read(&c, 1);
	}
	return input;
}

auto&
operator>>(In auto& input, char* str)
{
	do {
		input.read(str, 1);
	} while(*str++);
	return input;
}

auto&
operator>>(In auto& in, InFilter auto& inFilter) noexcept
{
	inFilter.mSource = &in;
	return inFilter;
}

auto&
operator>>(std::nullptr_t, InFilter auto& inFilter) noexcept
{
	static class : public Input {
		std::size_t
		readBytes(std::byte* dest, std::size_t size) override
		{ throw Exception(std::make_error_code(static_cast<std::errc>(ENODATA))); }
	} nullInput;
	return nullInput >> inFilter;
}

/**
 * @brief	Deserializable concept
 * @tparam	T
 * @tparam	InputType Derived Input stream type
 * @details	Requires T has an <b>InputType& operator>>(InputType&, T&)</b> overload
 */
template <typename T, typename InputType>
concept Deserializable = In<InputType> && requires(InputType& is, T& t)
{{ is >> t } -> std::same_as<InputType&>; };

template <typename T, typename InputRef>
concept DeserializableR = Deserializable<T, std::remove_reference_t<InputRef>>;

/**
 * @brief	Constructible concept
 * @tparam	T
 * @tparam	InputType Derived Input stream type
 * @tparam	Args Optional additional parameter types to be used to construct T
 * @details	Requires T has a <b>T(InputType&, Args ...)</b> constructor
 */
template <typename T, typename InputType, typename ... Args>
concept Constructible = In<InputType> && std::constructible_from<T, InputType&, Args ...>;

template <typename T, typename InputRef, typename ... Args>
concept ConstructibleR = Constructible<T, std::remove_reference_t<InputRef>, Args ...>;

/**
 * @brief	Extractable concept
 * @tparam	T
 * @tparam	Args Optional parameter types to be used to construct T
 * @details	Requires T has either trivial default constructor or a <b>T(Args ...)</b> constructor
 * 			and <b>In& operator>>(In&, T&)</b> overload
 */
template <typename T, typename InputType, typename ... Args>
concept Extractable = Deserializable<T, InputType> && (std::is_trivially_default_constructible_v<T> || std::constructible_from<T, Args ...>);

template <typename T, typename InputRef, typename ... Args>
concept ExtractableR = Extractable<T, std::remove_reference_t<InputRef>, Args ...>;

template <typename T, typename ... Args>
T
Get(In auto& input, Args&& ... args)
requires std::constructible_from<T, decltype(input), Args ...>
{ return T(input, std::forward<Args>(args) ...); }

template <typename T, typename ... Args>
T
Get(In auto& input, Args&& ... args)
requires std::constructible_from<T, Args ...> && DeserializableR<T, decltype(input)>
{
	T t(std::forward<Args>(args) ...);
	input >> t;
	return t;
}

template <typename T>
T
Get(In auto& input)
requires std::is_trivially_default_constructible_v<T> && DeserializableR<T, decltype(input)>
{
	T t;
	input >> t;
	return t;
}

}//namespace Stream

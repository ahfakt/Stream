#ifndef STREAM_IN_OUT_H
#define STREAM_IN_OUT_H

#include <concepts>
#include <cstddef>
#include <string>
#include <system_error>
#include <utility>

namespace Stream {

class Input;

/**
 * @brief	Input stream base class
 * @class	Input InOut.h "Stream/InOut.h"
 */
class Input {
protected:
	Input() noexcept = default;

	virtual std::size_t
	readBytes(std::byte* dest, std::size_t size) = 0;

public:
	struct Exception : std::system_error {
		using std::system_error::system_error;

		enum class Code : int {
			Uninitialized = 1
		};

		Exception(Exception const& other) = default;

		[[nodiscard]] void*
		getUnreadBuffer() const noexcept;

		[[nodiscard]] std::size_t
		getUnreadSize() const noexcept;
	private:
		friend class Input;
		void* mDest;
		std::size_t mSize;
	};//struct Exception

	Input(Input const&) = delete;

	Input&
	read(void* dest, std::size_t size);

	std::size_t
	readSome(void* dest, std::size_t size);
};//class Input

template <typename T>
Input&
operator>>(Input& input, T& t)
requires std::is_trivially_copyable_v<T>
{ return input.read(reinterpret_cast<void*>(&t), sizeof(T)); }

Input&
operator>>(Input& input, std::string& str);

Input&
operator>>(Input& input, char* str);

/**
 * @brief	Output stream base class
 * @class	Output InOut.h "Stream/InOut.h"
 */
class Output {
protected:
	Output() noexcept = default;

	virtual std::size_t
	writeBytes(std::byte const* src, std::size_t size) = 0;

	virtual void
	flush();

public:
	struct Exception : std::system_error {
		using std::system_error::system_error;

		enum class Code : int {
			Uninitialized = 1
		};

		Exception(Exception const& other) = default;

		[[nodiscard]] void const*
		getUnwrittenBuffer() const noexcept;

		[[nodiscard]] std::size_t
		getUnwrittenSize() const noexcept;
	private:
		friend class Output;
		void const* mSrc;
		std::size_t mSize;
	};//struct Exception

	Output(Output const&) = delete;

	Output&
	write(void const* src, std::size_t size);

	std::size_t
	writeSome(void const* src, std::size_t size);

	Output&
	operator<<(std::nullptr_t);
};//class Output

template <typename T>
Output&
operator<<(Output& output, T const& t)
requires std::is_trivially_copyable_v<T>
{ return output.write(reinterpret_cast<void const*>(&t), sizeof(T)); }

Output&
operator<<(Output& output, std::string const& str);

Output&
operator<<(Output& output, char const* str);

/**
 * @brief	Input stream filter base class
 * @class	InputFilter InOut.h "Stream/InOut.h"
 */
class InputFilter : public Input {
protected:
	Input* mSource;

	InputFilter() noexcept;

public:
	friend void
	swap(InputFilter& a, InputFilter& b) noexcept;

	friend InputFilter&
	operator>>(Input& input, InputFilter& inputFilter) noexcept;

	friend InputFilter&
	operator>>(std::nullptr_t, InputFilter& inputFilter) noexcept;
};//class InputFilter

/**
 * @brief	Output stream filter base class
 * @class	OutputFilter InOut.h "Stream/InOut.h"
 */
class OutputFilter : public Output {
protected:
	Output* mSink;

	OutputFilter() noexcept;

public:
	friend void
	swap(OutputFilter& a, OutputFilter& b) noexcept;

	friend OutputFilter&
	operator<<(Output& output, OutputFilter& outputFilter) noexcept;

	friend OutputFilter&
	operator<<(std::nullptr_t, OutputFilter& outputFilter) noexcept;
};//class OutputFilter

template <typename T>
concept InType = std::is_base_of_v<Input, T>;

template <typename T>
concept OutType = std::is_base_of_v<Output, T>;

template <typename T>
concept InOutType = InType<T> && OutType<T>;

template <typename T>
concept InFilterType = std::is_base_of_v<InputFilter, T>;

template <typename T>
concept OutFilterType = std::is_base_of_v<OutputFilter, T>;

template <typename T>
concept InOutFilterType = InFilterType<T> && OutFilterType<T>;

/**
 * @brief	Deserializable concept
 * @tparam	T
 * @tparam	InputType Derived Input stream type
 * @details	Requires T has an <b>InputType& operator>>(InputType&, T&)</b> overload
 */
template <typename T, typename InputType>
concept Deserializable = InType<InputType> && requires(InputType& is, T& t)
{{ is >> t } -> std::same_as<InputType&>; };

template <typename T, typename InputRef>
concept DeserializableR = Deserializable<T, std::remove_reference_t<InputRef>>;

/**
 * @brief	Serializable concept
 * @tparam	T
 * @tparam	OutputType Derived Output stream type
 * @details	Requires T has an <b>OutputType& operator<<(OutputType&, T const&)</b> overload
 */
template <typename T, typename OutputType>
concept Serializable = OutType<OutputType> && requires(OutputType& os, T const& t)
{{ os << t } -> std::same_as<OutputType&>; };

template <typename T, typename OutputRef>
concept SerializableR = Serializable<T, std::remove_reference_t<OutputRef>>;

/**
 * @brief	Constructible concept
 * @tparam	T
 * @tparam	InputType Derived Input stream type
 * @tparam	Args Optional additional parameter types to be used to construct T
 * @details	Requires T has a <b>T(InputType&, Args ...)</b> constructor
 */
template <typename T, typename InputType, typename ... Args>
concept Constructible = InType<InputType> && std::constructible_from<T, InputType&, Args ...>;

template <typename T, typename InputRef, typename ... Args>
concept ConstructibleR = Constructible<T, std::remove_reference_t<InputRef>, Args ...>;

/**
 * @brief	Extractable concept
 * @tparam	T
 * @tparam	Args Optional parameter types to be used to construct T
 * @details	Requires T has either trivial default constructor or a <b>T(Args ...)</b> constructor
 * 			and <b>InType& operator>>(InType&, T&)</b> overload
 */
template <typename T, typename InputType, typename ... Args>
concept Extractable = Deserializable<T, InputType> && (std::is_trivially_default_constructible_v<T> || std::constructible_from<T, Args ...>);

template <typename T, typename InputRef, typename ... Args>
concept ExtractableR = DeserializableR<T, InputRef> && (std::is_trivially_default_constructible_v<T> || std::constructible_from<T, Args ...>);

template <typename T, typename ... Args>
T
Get(InType auto& input, Args&& ... args)
requires std::constructible_from<T, decltype(input), Args ...>
{ return T(input, std::forward<Args>(args) ...); }

template <typename T, typename ... Args>
T
Get(InType auto& input, Args&& ... args)
requires std::constructible_from<T, Args ...> && DeserializableR<T, decltype(input)>
{
	T t(std::forward<Args>(args) ...);
	input >> t;
	return t;
}

template <typename T>
T
Get(InType auto& input)
requires std::is_trivially_default_constructible_v<T> && DeserializableR<T, decltype(input)>
{
	T t;
	input >> t;
	return t;
}

auto&
operator<=>(InOutType auto& inOut, InOutFilterType auto& inOutFilter) noexcept
{
	inOut >> inOutFilter;
	inOut << inOutFilter;
	return inOutFilter;
}

auto&
operator<=>(std::nullptr_t, InOutFilterType auto& inOutFilter) noexcept
{
	nullptr >> inOutFilter;
	nullptr << inOutFilter;
	return inOutFilter;
}

std::error_code
make_error_code(Input::Exception::Code e) noexcept;

std::error_code
make_error_code(Output::Exception::Code e) noexcept;

}//namespace Stream

namespace std {

template <>
struct is_error_code_enum<Stream::Input::Exception::Code> : true_type {};

template <>
struct is_error_code_enum<Stream::Output::Exception::Code> : true_type {};

}//namespace std

#endif //STREAM_IN_OUT_H

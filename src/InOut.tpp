#pragma once

#include "Stream/InOut.hpp"
#include <cstdint>


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
 * Initialize a T object with input and optional additional args
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
 * Initialize a T object with optional args and extract it from input
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


template <typename S, typename T>
T&
operator>(S& source, T& input) noexcept
requires (std::derived_from<S, Input> &&
	std::derived_from<T, Input>)
{
	if (input.mSource) // subclass can explicitly disallow linking to another source
		input.mSource = &source;
	return input;
}

template <typename T>
T&
operator>(std::nullptr_t, T& input) noexcept
requires std::derived_from<T, Input>
{ return *Input::Unreadable > input; }


template <typename S, typename T>
T&
operator<(S& sink, T& output) noexcept
requires (std::derived_from<S, Output> &&
	std::derived_from<T, Output>)
{
	if (output.mSink) // subclass can explicitly disallow linking to another sink
		output.mSink = &sink;
	return output;
}

template <typename T>
T&
operator<(std::nullptr_t, T& output) noexcept
requires std::derived_from<T, Output>
{ return *Output::Unwritable < output; }

/**
 * Link @p inputOutput to @p sourceSink for input and output
 */
template <typename S, typename T>
T&
operator|(S& sourceSink, T& inputOutput) noexcept
requires (std::derived_from<S, Input> && std::derived_from<S, Output> &&
	std::derived_from<T, Input> && std::derived_from<T, Output>)

{
	static_cast<Input& >(sourceSink) > static_cast<Input& >(inputOutput);
	static_cast<Output&>(sourceSink) < static_cast<Output&>(inputOutput);
	return inputOutput;
}

/**
 * Link @p inputOutput to an unreadable source and to an unwritable sink
 */
template <typename S, typename T>
T&
operator|(std::nullptr_t, T& inputOutput) noexcept
requires (std::derived_from<T, Input> && std::derived_from<T, Output>)
{
	nullptr > static_cast<Input& >(inputOutput);
	nullptr < static_cast<Output&>(inputOutput);
	return inputOutput;
}

#if defined (NDEBUG)
#	define LOG_ERR(err) try { Stream::StdErr << __PRETTY_FUNCTION__ << ' ' << err << '\n'; } catch (...) {}
#else
#	define LOG_ERR(err) try { Stream::StdErr << __FILE_NAME__ << ':' << __LINE__ << ' ' << __PRETTY_FUNCTION__ << ' ' << err << '\n'; } catch (...) {}
#endif

}//namespace Stream

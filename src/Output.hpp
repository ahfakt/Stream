#include "Stream/Output.h"
#include <cstring>

namespace Stream {

auto&
operator<<(Out auto& output, auto const& t)
requires std::is_trivially_copyable_v<decltype(t)>
{
	output.write(reinterpret_cast<void const*>(&t), sizeof(t));
	return output;
}

auto&
operator<<(Out auto& output, std::string const& str)
{
	output.write(str.c_str(), str.size() + 1);
	return output;
}

auto&
operator<<(Out auto& output, char const* str)
{
	output.write(str, std::strlen(str) + 1);
	return output;
}

auto&
operator<<(Out auto& out, OutFilter auto& outFilter) noexcept
{
	outFilter.mSink = &out;
	return outFilter;
}

auto&
operator<<(std::nullptr_t, OutFilter auto& outFilter) noexcept
{
	static class : public Output {
		std::size_t
		writeBytes(std::byte const* src, std::size_t size) override
		{ throw Exception(std::make_error_code(static_cast<std::errc>(ENOSPC))); }
	} nullOutput;
	return nullOutput << outFilter;
}

/**
 * @brief	Serializable concept
 * @tparam	T
 * @tparam	OutputType Derived Output stream type
 * @details	Requires T has an <b>OutputType& operator<<(OutputType&, T const&)</b> overload
 */
template <typename T, typename OutputType>
concept Serializable = Out<OutputType> && requires(OutputType& os, T const& t)
{{ os << t } -> std::same_as<OutputType&>; };

template <typename T, typename OutputRef>
concept SerializableR = Serializable<T, std::remove_reference_t<OutputRef>>;

}//namespace Stream

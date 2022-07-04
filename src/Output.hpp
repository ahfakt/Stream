#include "Stream/Output.h"
#include <cstring>

namespace std {

template <>
struct is_error_code_enum<Stream::Output::Exception::Code> : true_type {};

}//namespace std

namespace Stream {

template <typename T>
concept Out = std::is_base_of_v<Output, T>;

template <typename T>
concept OutFilter = std::is_base_of_v<OutputFilter, T>;

Output&
Output::operator<<(auto const& t)
requires std::is_trivially_copyable_v<std::remove_cvref_t<decltype(t)>>
{ return write(reinterpret_cast<void const*>(&t), sizeof(t)); }

/**
 * @brief	Serializable concept
 * @tparam	T
 * @tparam	OutputType Derived Output stream type
 * @details	Requires T has an <b>OutputType& operator<<(OutputType&, T const&)</b> overload
 */
template <typename T, typename OutputRef>
concept Serializable = Out<std::remove_cvref_t<OutputRef>> && requires(OutputRef os, T const& t)
{{ os << t } -> std::same_as<OutputRef>; };

}//namespace Stream

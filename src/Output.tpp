#include "Stream/Output.hpp"
#include <cstring>

namespace std {

template <>
struct is_error_code_enum<Stream::Output::Exception::Code> : true_type {};

}//namespace std

namespace Stream {

template <typename T, typename OutputType>
concept InsertableTo =
	std::derived_from<OutputType, Output> &&
	std::derived_from<OutputType, std::remove_reference_t<decltype(std::declval<OutputType&>() << std::declval<T const&>())>>;

Output&
Output::operator<<(auto const& t)
requires std::is_trivially_copyable_v<std::remove_reference_t<decltype(t)>>
{ return write(reinterpret_cast<void const*>(&t), sizeof t); }

}//namespace Stream

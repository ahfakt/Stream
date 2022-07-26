#include "Stream/Input.h"
#include "Stream/Output.h"

namespace Stream {

template <typename T>
concept InOut = std::derived_from<T, Input> && std::derived_from<T, Output>;

template <typename T>
concept InOutFilter = std::derived_from<T, InputFilter> && std::derived_from<T, OutputFilter>;

auto&
operator<=>(InOut auto& inOut, InOutFilter auto& inOutFilter) noexcept
{
	inOut >> inOutFilter;
	inOut << inOutFilter;
	return inOutFilter;
}

auto&
operator<=>(std::nullptr_t, InOutFilter auto& inOutFilter) noexcept
{
	nullptr >> inOutFilter;
	nullptr << inOutFilter;
	return inOutFilter;
}

}//namespace Stream

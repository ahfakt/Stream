#include "Stream/Input.h"
#include "Stream/Output.h"

namespace Stream {

template <typename T>
concept InOut = In<T> && Out<T>;

template <typename T>
concept InOutFilter = InFilter<T> && OutFilter<T>;

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

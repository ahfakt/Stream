#include "Stream/Transparent.hpp"

namespace Stream {

auto&
operator>(Input& input, std::derived_from<TransparentInput> auto& transparentInput) noexcept
{
	input > static_cast<TransparentInput&>(transparentInput);
	return transparentInput;
}

auto&
operator>(std::nullptr_t, std::derived_from<TransparentInput> auto& transparentInput) noexcept
{
	nullptr > static_cast<TransparentInput&>(transparentInput);
	return transparentInput;
}

auto&
operator<(Output& output, std::derived_from<TransparentOutput> auto& transparentOutput) noexcept
{
	output < static_cast<TransparentOutput&>(transparentOutput);
	return transparentOutput;
}

auto&
operator<(std::nullptr_t, std::derived_from<TransparentOutput> auto& transparentOutput) noexcept
{
	nullptr < static_cast<TransparentOutput&>(transparentOutput);
	return transparentOutput;
}

template <typename T>
concept InOut = std::derived_from<T, Input> && std::derived_from<T, Output>;

template <typename T>
concept TransparentInOut = std::derived_from<T, TransparentInput> && std::derived_from<T, TransparentOutput>;

auto&
operator<=>(InOut auto& inOut, TransparentInOut auto& transparentInOut) noexcept
{
	static_cast<Input&>(inOut) > static_cast<TransparentInput&>(transparentInOut);
	static_cast<Output&>(inOut) < static_cast<TransparentOutput&>(transparentInOut);
	return transparentInOut;
}

auto&
operator<=>(std::nullptr_t, TransparentInOut auto& transparentInOut) noexcept
{
	nullptr > static_cast<TransparentInput&>(transparentInOut);
	nullptr < static_cast<TransparentOutput&>(transparentInOut);
	return transparentInOut;
}

}//namespace Stream
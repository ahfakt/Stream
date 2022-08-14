#include "Stream/Transform.hpp"

namespace Stream {

auto&
operator>(BufferInput& bufferInput, std::derived_from<TransformInput> auto& transformInput) noexcept
{
	bufferInput > static_cast<TransformInput&>(transformInput);
	return transformInput;
}

auto&
operator>(std::nullptr_t, std::derived_from<TransformInput> auto& transformInput) noexcept
{
	nullptr > static_cast<TransformInput&>(transformInput);
	return transformInput;
}

auto&
operator<(BufferOutput& bufferOutput, std::derived_from<TransformOutput> auto& transformOutput) noexcept
{
	bufferOutput < static_cast<TransformOutput&>(transformOutput);
	return transformOutput;
}

auto&
operator<(std::nullptr_t, std::derived_from<TransformOutput> auto& transformOutput) noexcept
{
	nullptr < static_cast<TransformOutput&>(transformOutput);
	return transformOutput;
}

template <typename T>
concept BufferInOut = std::derived_from<T, BufferInput> && std::derived_from<T, BufferOutput>;

template <typename T>
concept TransformInOut = std::derived_from<T, TransformInput> && std::derived_from<T, TransformOutput>;

auto&
operator<=>(BufferInOut auto& bufferInOut, TransformInOut auto& transformInOut) noexcept
{
	static_cast<BufferInput&>(bufferInOut) > static_cast<TransformInput&>(transformInOut);
	static_cast<BufferOutput&>(bufferInOut) < static_cast<TransformOutput&>(transformInOut);
	return transformInOut;
}

auto&
operator<=>(std::nullptr_t, TransformInOut auto& transformInOut) noexcept
{
	nullptr > static_cast<TransformInput&>(transformInOut);
	nullptr < static_cast<TransformOutput&>(transformInOut);
	return transformInOut;
}

}//namespace Stream
#include "Stream/Raw.h"

namespace Stream {

Raw::Raw(void* pos, std::size_t size) noexcept:
	pos(reinterpret_cast<std::byte*>(pos)), size(size) {}

}//namespace Stream

#ifndef STREAM_RAW_H
#define STREAM_RAW_H

#include <cstddef>

namespace Stream {

class Raw {
	friend class Input;
	friend class Output;
	std::byte* pos;
	std::size_t size;
public:
	Raw(void*, std::size_t) noexcept;
};//class Raw

}//namespace Stream

#endif //STREAM_RAW_H

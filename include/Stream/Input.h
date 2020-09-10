#ifndef STREAM_INPUT_H
#define STREAM_INPUT_H

#include <utility>
#include "Raw.h"

namespace Stream {

class Input {
protected:
	std::byte *mGetBegin, *mGetCurrent, *mGetEnd;
	union {
		char state;
		struct {
			bool err:1;
			bool eof:1;
		};
	};
	virtual bool load() = 0; // True on fail
	Input() noexcept;
public:
	Input& operator>>(bool&);
	Input& operator>>(char&);
	Input& operator>>(unsigned char&);
	Input& operator>>(short&);
	Input& operator>>(unsigned short&);
	Input& operator>>(int&);
	Input& operator>>(unsigned int&);
	Input& operator>>(long&);
	Input& operator>>(unsigned long&);
	Input& operator>>(long long&);
	Input& operator>>(unsigned long long&);
	Input& operator>>(float&);
	Input& operator>>(double&);
	Input& operator>>(Raw);

	class Exception {
		const char* message;
	public:
		Exception(const char*);
		operator const char*() const noexcept;
	};

	explicit operator bool() const noexcept;
};//class Input

template <typename, typename = std::void_t<>>
inline constexpr bool isDeserializable = false;
template <typename T>
inline constexpr bool isDeserializable<T,
		std::void_t<decltype(T(std::declval<Input&>()))>
> = true;

}//namespace Stream

#endif //STREAM_INPUT_H

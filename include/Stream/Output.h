#ifndef STREAM_OUTPUT_H
#define STREAM_OUTPUT_H

#include <utility>
#include "Raw.h"

namespace Stream {

class Output {
protected:
	std::byte *mPutBegin, *mPutCurrent, *mPutEnd;
	union {
		char state;
		struct {
			bool err:1;
			bool app:1;
		};
	};
	virtual bool store() = 0; // True on fail
	Output() noexcept;
public:
	Output& operator<<(bool);
	Output& operator<<(char);
	Output& operator<<(unsigned char);
	Output& operator<<(short);
	Output& operator<<(unsigned short);
	Output& operator<<(int);
	Output& operator<<(unsigned int);
	Output& operator<<(long);
	Output& operator<<(unsigned long);
	Output& operator<<(long long);
	Output& operator<<(unsigned long long);
	Output& operator<<(float);
	Output& operator<<(double);
	Output& operator<<(Raw);

	class Exception {
		const char* message;
	public:
		Exception(const char*);
		operator const char*() const noexcept;
	};

	explicit operator bool() const noexcept;
};//class Output

template <typename, typename = std::void_t<>>
inline constexpr bool isSerializable = false;
template <typename T>
inline constexpr bool isSerializable<T,
		std::void_t<decltype(std::declval<Output&>() << std::declval<T const&>())>
> = true;

}//namespace Stream

#endif //STREAM_OUTPUT_H

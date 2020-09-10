#include "Stream/Input.h"
#include <cstring>

namespace Stream {

Input::Input() noexcept :
	mGetBegin(nullptr), mGetCurrent(nullptr), mGetEnd(nullptr), err(false), eof(false) {}

Input&
Input::operator>>(bool& val) {
	if (mGetEnd - mGetCurrent < sizeof(bool) && load()) return *this;
	val = reinterpret_cast<bool&>(*mGetCurrent);
	mGetCurrent += sizeof(bool);
	return *this;
}

Input&
Input::operator>>(char& val) {
	if (mGetEnd - mGetCurrent < sizeof(char) && load()) return *this;
	val = reinterpret_cast<char&>(*mGetCurrent);
	mGetCurrent += sizeof(char);
	return *this;
}

Input&
Input::operator>>(unsigned char& val) {
	if (mGetEnd - mGetCurrent < sizeof(unsigned char) && load()) return *this;
	val = reinterpret_cast<unsigned char&>(*mGetCurrent);
	mGetCurrent += sizeof(unsigned char);
	return *this;
}

Input&
Input::operator>>(short& val) {
	if (mGetEnd - mGetCurrent < sizeof(short)) {
		char* v = reinterpret_cast<char*>(&val);
		operator>>(v[0]);
		return eof ? *this : operator>>(v[1]);
	}
	val = reinterpret_cast<short&>(*mGetCurrent);
	mGetCurrent += sizeof(short);
	return *this;
}

Input&
Input::operator>>(unsigned short& val) {
	if (mGetEnd - mGetCurrent < sizeof(unsigned short)) {
		unsigned char* v = reinterpret_cast<unsigned char*>(&val);
		operator>>(v[0]);
		return eof ? *this : operator>>(v[1]);
	}
	val = reinterpret_cast<unsigned short&>(*mGetCurrent);
	mGetCurrent += sizeof(unsigned short);
	return *this;
}

Input&
Input::operator>>(int& val) {
	if (mGetEnd - mGetCurrent < sizeof(int)) {
		if constexpr (sizeof(int) > sizeof(short)) {
			short* v = reinterpret_cast<short*>(&val);
			operator>>(v[0]);
			return eof ? *this : operator>>(v[1]);
		}
		else {
			char* v = reinterpret_cast<char*>(&val);
			operator>>(v[0]);
			return eof ? *this : operator>>(v[1]);
		}
	}
	val = reinterpret_cast<int&>(*mGetCurrent);
	mGetCurrent += sizeof(int);
	return *this;
}

Input&
Input::operator>>(unsigned int& val) {
	if (mGetEnd - mGetCurrent < sizeof(unsigned int)) {
		if constexpr (sizeof(unsigned int) > sizeof(unsigned short)) {
			unsigned short* v = reinterpret_cast<unsigned short*>(&val);
			operator>>(v[0]);
			return eof ? *this : operator>>(v[1]);
		}
		else {
			unsigned char* v = reinterpret_cast<unsigned char*>(&val);
			operator>>(v[0]);
			return eof ? *this : operator>>(v[1]);
		}
	}
	val = reinterpret_cast<unsigned int&>(*mGetCurrent);
	mGetCurrent += sizeof(unsigned int);
	return *this;
}

Input&
Input::operator>>(long& val) {
	if (mGetEnd - mGetCurrent < sizeof(long)) {
		if constexpr (sizeof(long) > sizeof(int)) {
			int* v = reinterpret_cast<int*>(&val);
			operator>>(v[0]);
			return eof ? *this : operator>>(v[1]);
		}
		else {
			short* v = reinterpret_cast<short*>(&val);
			operator>>(v[0]);
			return eof ? *this : operator>>(v[1]);
		}
	}
	val = reinterpret_cast<long&>(*mGetCurrent);
	mGetCurrent += sizeof(long);
	return *this;
}

Input&
Input::operator>>(unsigned long& val) {
	if (mGetEnd - mGetCurrent < sizeof(unsigned long)) {
		if constexpr (sizeof(unsigned long) > sizeof(unsigned int)) {
			unsigned int* v = reinterpret_cast<unsigned int*>(&val);
			operator>>(v[0]);
			return eof ? *this : operator>>(v[1]);
		}
		else {
			unsigned short* v = reinterpret_cast<unsigned short*>(&val);
			operator>>(v[0]);
			return eof ? *this : operator>>(v[1]);
		}
	}
	val = reinterpret_cast<unsigned long&>(*mGetCurrent);
	mGetCurrent += sizeof(unsigned long);
	return *this;
}

Input&
Input::operator>>(long long& val) {
	if (mGetEnd - mGetCurrent < sizeof(long long)) {
		if constexpr (sizeof(long long) > sizeof(long)) {
			long* v = reinterpret_cast<long*>(&val);
			operator>>(v[0]);
			return eof ? *this : operator>>(v[1]);
		}
		else {
			int* v = reinterpret_cast<int*>(&val);
			operator>>(v[0]);
			return eof ? *this : operator>>(v[1]);
		}
	}
	val = reinterpret_cast<long long&>(*mGetCurrent);
	mGetCurrent += sizeof(long long);
	return *this;
}

Input&
Input::operator>>(unsigned long long& val) {
	if (mGetEnd - mGetCurrent < sizeof(unsigned long long)) {
		if constexpr (sizeof(unsigned long long) > sizeof(unsigned long)) {
			unsigned long* v = reinterpret_cast<unsigned long*>(&val);
			operator>>(v[0]);
			return eof ? *this : operator>>(v[1]);
		}
		else {
			unsigned int* v = reinterpret_cast<unsigned int*>(&val);
			operator>>(v[0]);
			return eof ? *this : operator>>(v[1]);
		}
	}
	val = reinterpret_cast<unsigned long long&>(*mGetCurrent);
	mGetCurrent += sizeof(unsigned long long);
	return *this;
}

Input&
Input::operator>>(float& val) {
	if (mGetEnd - mGetCurrent < sizeof(float)) {
		short* v = reinterpret_cast<short*>(&val);
		operator>>(v[0]);
		return eof ? *this : operator>>(v[1]);
	}
	val = reinterpret_cast<float&>(*mGetCurrent);
	mGetCurrent += sizeof(float);
	return *this;
}

Input&
Input::operator>>(double& val) {
	if (mGetEnd - mGetCurrent < sizeof(double)) {
		float* v = reinterpret_cast<float*>(&val);
		operator>>(v[0]);
		return eof ? *this : operator>>(v[1]);
	}
	val = reinterpret_cast<double&>(*mGetCurrent);
	mGetCurrent += sizeof(double);
	return *this;
}

Input&
Input::operator>>(Raw raw) {
	while (mGetEnd - mGetCurrent < raw.size) {
		std::size_t count = mGetEnd - mGetCurrent;
		std::memcpy(raw.pos, mGetCurrent, count);
		mGetCurrent += count;
		raw.pos += count;
		raw.size -= count;
		if (load()) return *this;
	}
	if (raw.size) {
		std::memcpy(raw.pos, mGetCurrent, raw.size);
		mGetCurrent += raw.size;
	}
	return *this;
}

Input::Exception::Exception(const char * str):
		message(str) {}

Input::Exception::operator const char *() const noexcept
{ return message; }

Input::operator bool() const noexcept
{ return !state; }

}//namespace Stream

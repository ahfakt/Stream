#include "Stream/Output.h"
#include <cstring>

namespace Stream {

Output::Output() noexcept :
	mPutBegin(nullptr), mPutCurrent(nullptr), mPutEnd(nullptr), err(false), app(false) {}

Output&
Output::operator<<(bool val) {
	if (mPutEnd - mPutCurrent < sizeof(bool) && store()) return *this;
	reinterpret_cast<bool&>(*mPutCurrent) = val;
	mPutCurrent += sizeof(bool);
	return *this;
}

Output&
Output::operator<<(char val) {
	if (mPutEnd - mPutCurrent < sizeof(char) && store()) return *this;
	reinterpret_cast<char&>(*mPutCurrent) = val;
	mPutCurrent += sizeof(char);
	return *this;
}

Output&
Output::operator<<(unsigned char val) {
	if (mPutEnd - mPutCurrent < sizeof(unsigned char) && store()) return *this;
	reinterpret_cast<unsigned char&>(*mPutCurrent) = val;
	mPutCurrent += sizeof(unsigned char);
	return *this;
}

Output&
Output::operator<<(short val) {
	if (mPutEnd - mPutCurrent < sizeof(short)) {
		char* v = reinterpret_cast<char*>(&val);
		operator<<(v[0]);
		return err ? *this : operator<<(v[1]);
	}
	reinterpret_cast<short&>(*mPutCurrent) = val;
	mPutCurrent += sizeof(short);
	return *this;
}

Output&
Output::operator<<(unsigned short val) {
	if (mPutEnd - mPutCurrent < sizeof(unsigned short)) {
		unsigned char* v = reinterpret_cast<unsigned char*>(&val);
		operator<<(v[0]);
		return err ? *this : operator<<(v[1]);
	}
	reinterpret_cast<unsigned short&>(*mPutCurrent) = val;
	mPutCurrent += sizeof(unsigned short);
	return *this;
}

Output&
Output::operator<<(int val) {
	if (mPutEnd - mPutCurrent < sizeof(int)) {
		if constexpr (sizeof(int) > sizeof(short)) {
			short* v = reinterpret_cast<short*>(&val);
			operator<<(v[0]);
			return err ? *this : operator<<(v[1]);
		}
		else {
			char* v = reinterpret_cast<char*>(&val);
			operator<<(v[0]);
			return err ? *this : operator<<(v[1]);
		}
	}
	reinterpret_cast<int&>(*mPutCurrent) = val;
	mPutCurrent += sizeof(int);
	return *this;
}

Output&
Output::operator<<(unsigned int val) {
	if (mPutEnd - mPutCurrent < sizeof(unsigned int)) {
		if constexpr (sizeof(unsigned int) > sizeof(unsigned short)) {
			unsigned short* v = reinterpret_cast<unsigned short*>(&val);
			operator<<(v[0]);
			return err ? *this : operator<<(v[1]);
		}
		else {
			unsigned char* v = reinterpret_cast<unsigned char*>(&val);
			operator<<(v[0]);
			return err ? *this : operator<<(v[1]);
		}
	}
	reinterpret_cast<unsigned int&>(*mPutCurrent) = val;
	mPutCurrent += sizeof(unsigned int);
	return *this;
}

Output&
Output::operator<<(long val) {
	if (mPutEnd - mPutCurrent < sizeof(long)) {
		if constexpr (sizeof(long) > sizeof(int)) {
			int* v = reinterpret_cast<int*>(&val);
			operator<<(v[0]);
			return err ? *this : operator<<(v[1]);
		}
		else {
			short* v = reinterpret_cast<short*>(&val);
			operator<<(v[0]);
			return err ? *this : operator<<(v[1]);
		}
	}
	reinterpret_cast<long&>(*mPutCurrent) = val;
	mPutCurrent += sizeof(long);
	return *this;
}

Output&
Output::operator<<(unsigned long val) {
	if (mPutEnd - mPutCurrent < sizeof(unsigned long)) {
		if constexpr (sizeof(unsigned long) > sizeof(unsigned int)) {
			unsigned int* v = reinterpret_cast<unsigned int*>(&val);
			operator<<(v[0]);
			return err ? *this : operator<<(v[1]);
		}
		else {
			unsigned short* v = reinterpret_cast<unsigned short*>(&val);
			operator<<(v[0]);
			return err ? *this : operator<<(v[1]);
		}
	}
	reinterpret_cast<unsigned long&>(*mPutCurrent) = val;
	mPutCurrent += sizeof(unsigned long);
	return *this;
}

Output&
Output::operator<<(long long val) {
	if (mPutEnd - mPutCurrent < sizeof(long long)) {
		if constexpr (sizeof(long long) > sizeof(long)) {
			long* v = reinterpret_cast<long*>(&val);
			operator<<(v[0]);
			return err ? *this : operator<<(v[1]);
		}
		else {
			int* v = reinterpret_cast<int*>(&val);
			operator<<(v[0]);
			return err ? *this : operator<<(v[1]);
		}
	}
	reinterpret_cast<long long&>(*mPutCurrent) = val;
	mPutCurrent += sizeof(long long);
	return *this;
}

Output&
Output::operator<<(unsigned long long val) {
	if (mPutEnd - mPutCurrent < sizeof(unsigned long long)) {
		if constexpr (sizeof(unsigned long long) > sizeof(unsigned long)) {
			unsigned long* v = reinterpret_cast<unsigned long*>(&val);
			operator<<(v[0]);
			return err ? *this : operator<<(v[1]);
		}
		else {
			unsigned int* v = reinterpret_cast<unsigned int*>(&val);
			operator<<(v[0]);
			return err ? *this : operator<<(v[1]);
		}
	}
	reinterpret_cast<unsigned long long&>(*mPutCurrent) = val;
	mPutCurrent += sizeof(unsigned long long);
	return *this;
}

Output&
Output::operator<<(float val) {
	if (mPutEnd - mPutCurrent < sizeof(float)) {
		short* v = reinterpret_cast<short*>(&val);
		operator<<(v[0]);
		return err ? *this : operator<<(v[1]);
	}
	reinterpret_cast<float&>(*mPutCurrent) = val;
	mPutCurrent += sizeof(float);
	return *this;
}

Output&
Output::operator<<(double val) {
	if (mPutEnd - mPutCurrent < sizeof(double)) {
		float* v = reinterpret_cast<float*>(&val);
		operator<<(v[0]);
		return err ? *this : operator<<(v[1]);
	}
	reinterpret_cast<double&>(*mPutCurrent) = val;
	mPutCurrent += sizeof(double);
	return *this;
}

Output&
Output::operator<<(Raw raw) {
	while (mPutEnd - mPutCurrent < raw.size) {
		std::size_t count = mPutEnd - mPutCurrent;
		std::memcpy(mPutCurrent, raw.pos, count);
		mPutCurrent += count;
		raw.pos += count;
		raw.size -= count;
		if (store()) return *this;
	}
	if (raw.size) {
		std::memcpy(mPutCurrent, raw.pos, raw.size);
		mPutCurrent += raw.size;
	}
	return *this;
}

Output::Exception::Exception(const char * str):
	message(str) {}

Output::Exception::operator const char *() const noexcept
{ return message; }

Output::operator bool() const noexcept
{ return !err; }

}//namespace Stream

#ifndef STREAM_INPUT_H
#define STREAM_INPUT_H

#include <cstddef>
#include <system_error>

namespace Stream {

/**
 * @brief	Input stream base class
 * @class	Input Input.h "Stream/Input.h"
 */
class Input {
protected:
	Input() noexcept = default;

	virtual std::size_t
	readBytes(std::byte* dest, std::size_t size) = 0;

public:
	struct Exception : std::system_error {
		using std::system_error::system_error;

		enum class Code : int {
			Uninitialized = 1
		};

		Exception(Exception const& other) = default;

		[[nodiscard]] void*
		getUnreadBuffer() const noexcept;

		[[nodiscard]] std::size_t
		getUnreadSize() const noexcept;
	private:
		friend class Input;
		void* mDest;
		std::size_t mSize;
	};//struct Exception

	Input(Input const&) = delete;

	Input&
	read(void* dest, std::size_t size);

	std::size_t
	readSome(void* dest, std::size_t size);
};//class Input

template <typename T>
concept In = std::is_base_of_v<Input, T>;

class InputFilter;

template <typename T>
concept InFilter = std::is_base_of_v<InputFilter, T>;

/**
 * @brief	Input stream filter base class
 * @class	InputFilter Input.h "Stream/Input.h"
 */
class InputFilter : public Input {
protected:
	Input* mSource;

	InputFilter() noexcept;

	std::size_t
	readBytes(std::byte* dest, std::size_t size) override;

public:
	friend void
	swap(InputFilter& a, InputFilter& b) noexcept;

	friend auto&
	operator>>(In auto& in, InFilter auto& inFilter) noexcept;
};//class InputFilter

std::error_code
make_error_code(Input::Exception::Code e) noexcept;

}//namespace Stream

namespace std {

template <>
struct is_error_code_enum<Stream::Input::Exception::Code> : true_type {};

}//namespace std

#include "../src/Input.hpp"

#endif //STREAM_INPUT_H

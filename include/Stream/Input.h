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

	Input&
	operator>>(std::string& str);

	Input&
	operator>>(char* str);

	Input&
	operator>>(auto& t)
	requires std::is_trivially_copyable_v<std::remove_reference_t<decltype(t)>>;
};//class Input

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

	friend InputFilter&
	operator>>(Input& input, InputFilter& inputFilter) noexcept;

	friend InputFilter&
	operator>>(std::nullptr_t, InputFilter& inputFilter) noexcept;
};//class InputFilter

std::error_code
make_error_code(Input::Exception::Code e) noexcept;

}//namespace Stream

#include "../../src/Input.hpp"

#endif //STREAM_INPUT_H

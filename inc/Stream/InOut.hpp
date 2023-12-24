#pragma once

#include <cstdint>
#include <system_error>

namespace Stream {

template <typename C>
concept Char =
	std::is_same_v<C, char> ||
	std::is_same_v<C, wchar_t> ||
	std::is_same_v<C, char8_t> ||
	std::is_same_v<C, char16_t> ||
	std::is_same_v<C, char32_t>;

template <typename I>
concept SignedInt =
	std::is_same_v<I, signed char> ||
	std::is_same_v<I, signed short> ||
	std::is_same_v<I, signed int> ||
	std::is_same_v<I, signed long> ||
	std::is_same_v<I, signed long long>;

template <typename I>
concept UnsignedInt =
	std::is_same_v<I, unsigned char> ||
	std::is_same_v<I, unsigned short> ||
	std::is_same_v<I, unsigned int> ||
	std::is_same_v<I, unsigned long> ||
	std::is_same_v<I, unsigned long long>;

template <typename I>
concept Integer = SignedInt<I> || UnsignedInt<I>;

/**
 * @brief	Input stream base class
 * @class	Input Input.hpp "Stream/Input.hpp"
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
	};//struct Stream::Input::Exception

	Input(Input const&) = delete;

	Input&
	read(void* dest, std::size_t size);

	std::size_t
	readSome(void* dest, std::size_t size);

	Input&
	operator>>(auto& t)
	requires std::is_trivially_copyable_v<std::remove_reference_t<decltype(t)>>;

	template <Char C>
	Input&
	operator>>(std::basic_string<C>& s);

	Input&
	operator>>(Char auto* s);
};//class Stream::Input

extern Input& In;

/**
 * @brief	Output stream base class
 * @class	Output Output.hpp "Stream/Output.hpp"
 */
class Output {
protected:
	Output() noexcept = default;

	virtual std::size_t
	writeBytes(std::byte const* src, std::size_t size) = 0;

public:
	struct Exception : std::system_error {
		using std::system_error::system_error;

		enum class Code : int {
			Uninitialized
		};

		Exception(Exception const& other) = default;

		[[nodiscard]] void const*
		getUnwrittenBuffer() const noexcept;

		[[nodiscard]] std::size_t
		getUnwrittenSize() const noexcept;

	private:
		friend class Output;
		void const* mSrc;
		std::size_t mSize;
	};//struct Stream::Output::Exception

	Output(Output const&) = delete;

	Output&
	write(void const* src, std::size_t size);

	std::size_t
	writeSome(void const* src, std::size_t size);

	virtual void
	flush();

	Output&
	operator<<(auto const& t)
	requires std::is_trivially_copyable_v<std::remove_reference_t<decltype(t)>>;

	template <Char C>
	Output&
	operator<<(std::basic_string<C> const& s);

	Output&
	operator<<(Char auto const* s);
};//class Stream::Output

extern Output& Out;
extern Output& Err;

std::error_code
make_error_code(Input::Exception::Code e) noexcept;

std::error_code
make_error_code(Output::Exception::Code e) noexcept;

}//namespace Stream

namespace std {

template <>
struct is_error_code_enum<Stream::Input::Exception::Code> : true_type {};

template <>
struct is_error_code_enum<Stream::Output::Exception::Code> : true_type {};

}//namespace std

#include "../../src/Stream/InOut.tpp"

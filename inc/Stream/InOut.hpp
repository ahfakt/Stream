#pragma once

#include <system_error>
#include <type_traits>


namespace Stream {

/**
 * Character type concept
 * @concept	Char InOut.hpp "Stream/InOut.hpp"
 * @see https://en.cppreference.com/w/cpp/language/types#:~:text=.-,Character%20types,-Character%20types%20are
 */
template <typename C>
concept Char =
	std::is_same_v<char, std::remove_cv_t<C>> ||
	std::is_same_v<wchar_t, std::remove_cv_t<C>> ||
	std::is_same_v<char8_t, std::remove_cv_t<C>> ||
	std::is_same_v<char16_t, std::remove_cv_t<C>> ||
	std::is_same_v<char32_t, std::remove_cv_t<C>>;


/**
 * Signed integer type concept
 * @concept	SignedInt InOut.hpp "Stream/InOut.hpp"
 * @see https://en.cppreference.com/w/cpp/language/types#:~:text=Integral%20types-,Standard%20integer%20types,-int%20%E2%80%94%20basic%20integer
 */
template <typename I>
concept SignedInteger =
	std::is_integral_v<I> &&
	std::is_signed_v<I> &&
	!Char<I>;


/**
 * Unsigned integer type concept
 * @concept	UnsignedInt InOut.hpp "Stream/InOut.hpp"
 * @see https://en.cppreference.com/w/cpp/language/types#:~:text=Integral%20types-,Standard%20integer%20types,-int%20%E2%80%94%20basic%20integer
 */
template <typename I>
concept UnsignedInteger =
	std::is_integral_v<I> &&
	!std::is_signed_v<I> &&
	!Char<I> &&
	!std::is_same_v<bool, std::remove_cv_t<I>>;


/**
 * Signed or unsigned integer type concept
 * @concept	Integer InOut.hpp "Stream/InOut.hpp"
 * @see https://en.cppreference.com/w/cpp/language/types#:~:text=Integral%20types-,Standard%20integer%20types,-int%20%E2%80%94%20basic%20integer
 */
template <typename I>
concept Integer =
	std::is_integral_v<I> &&
	!Char<I> &&
	!std::is_same_v<bool, std::remove_cv_t<I>>;


/**
 * Pointer type concept
 * @concept	Pointer InOut.hpp "Stream/InOut.hpp"
 */
template <typename T>
concept Pointer = std::is_pointer_v<T>;


/**
 * %Exception class that contains common exception codes
 * @class	Exception InOut.hpp "Stream/InOut.hpp"
 */
class Exception {
public:
	enum class Code : int {
		Uninitialized = 1
	};//enum class Stream::Exception::Code

};//class Stream::Exception


/**
 * %Input stream base class
 * @class	Input InOut.hpp "Stream/InOut.hpp"
 */
class Input {
	static Input* Unreadable;

	Input* mSource;

protected:

	/**
	 * Default constructor
	 */
	Input(bool allowLink = true) noexcept;

	/**
	 * Read @p size bytes into @p dest
	 * @param[out]	dest Memory address where the data to be read will be written
	 * @param[in]	size Number of bytes to be read
	 * @return		Number of bytes that can actually be read. 0 to signal for retry.
	 * @pre			@p dest must be a valid memory area
	 * @pre			@p size must be non-zero
	 * @throws		Input::Exception
	 */
	virtual std::size_t
	readBytes(std::byte* dest, std::size_t size) = 0;

	/**
	 * Finalize the ongoing process and read any remaining data
	 */
	virtual void
	drain();

	Input&
	getSource() noexcept;

public:

	/**
	 * Input exception
	 * @class	Exception InOut.hpp "Stream/InOut.hpp"
	 */
	class Exception : public std::system_error {

		friend class Input;
		void* mDest;
		std::size_t mSize;

	public:

		using std::system_error::system_error;

		/**
		 * Default copy constructor
		 */
		Exception(Exception const& other) = default;

		[[nodiscard]]
		void*
		getUnreadBuffer() const noexcept;

		[[nodiscard]]
		std::size_t
		getUnreadSize() const noexcept;

	};//class Stream::Input::Exception


	/**
	 * Move constructor
	 */
	Input(Input&& other) noexcept;

	friend void
	swap(Input& a, Input& b) noexcept;

	Input&
	operator=(Input&& other) noexcept;

	/**
	 * Link @p input to @p source
	 */
	template <typename S, typename T>
	friend T&
	operator>(S& source, T& input) noexcept
	requires (std::derived_from<S, Input> &&
		std::derived_from<T, Input>);

	/**
	 * Link @p input to an unreadable source
	 */
	template <typename T>
	friend T&
	operator>(std::nullptr_t, T& input) noexcept
	requires std::derived_from<T, Input>;

	/**
	 * Call @ref drain()
	 * @throws		Input::Exception
	 */
	Input&
	operator>>(std::nullptr_t);

	/**
	 * Call @ref drain() after triggering the previous input to do the same
	 * @throws		Input::Exception
	 */
	Input&
	operator>(std::nullptr_t);

	/**
	 * Read @p size bytes into @p dest
	 * @param[out]	dest Memory address where the data to be read will be written
	 * @param[in]	size Number of bytes to be read
	 * @return		Self-reference
	 * @pre			@p dest must be a valid memory area
	 * @pre			@p size should be non-zero
	 * @throws		Input::Exception
	 */
	Input&
	read(void* dest, std::size_t size);

	/**
	 * Read @p size bytes into @p dest
	 * @param[out]	dest Memory address where the data to be read will be written
	 * @param[in]	size Number of bytes to be read
	 * @return		Number of bytes that can actually be read
	 * @pre			@p dest must be a valid memory area
	 * @pre			@p size should be non-zero
	 * @throws		Input::Exception
	 */
	std::size_t
	readSome(void* dest, std::size_t size);

	/**
	 * Read into trivially copyable @p t
	 * @param[out]	t
	 * @return		Self-reference
	 * @throws		Input::Exception
	 */
	Input&
	operator>>(auto& t)
	requires std::is_trivially_copyable_v<std::remove_reference_t<decltype(t)>>;

	/**
	 * Read size prepended string into @p s
	 * @param[out]	s
	 * @return		Self-reference
	 * @throws		Input::Exception
	 */
	template <Char C>
	Input&
	operator>>(std::basic_string<C>& s);

	/**
	 * Read null terminated string into @p s
	 * @param[out]	s
	 * @return		Self-reference
	 * @pre			@p s must be large enough to store the string
	 * @throws		Input::Exception
	 */
	Input&
	operator>>(Char auto* s);

};//class Stream::Input


extern Input& StdIn;


/**
 * %Output stream base class
 * @class	Output InOut.hpp "Stream/InOut.hpp"
 */
class Output {
	static Output* Unwritable;

	Output* mSink;

protected:

	/**
	 * Default constructor
	 */
	Output(bool allowLink = true) noexcept;

	/**
	 * Write @p size bytes from @p src
	 * @param[in]	src Memory address where the data to be written will be read
	 * @param[in]	size Number of bytes to be written
	 * @return		Number of bytes that can actually be written
	 * @pre			@p src must be a valid memory area
	 * @pre			@p size must be non-zero
	 * @throws		Output::Exception
	 */
	virtual std::size_t
	writeBytes(std::byte const* src, std::size_t size) = 0;

	/**
	 * Finalize the ongoing process and write any remaining data
	 */
	virtual void
	flush();

	Output&
	getSink() noexcept;

public:

	/**
	 * Output exception
	 * @class	Exception InOut.hpp "Stream/InOut.hpp"
	 */
	class Exception : public std::system_error {

		friend class Output;
		void const* mSrc;
		std::size_t mSize;

	public:

		using std::system_error::system_error;

		/**
		 * Default copy constructor
		 */
		Exception(Exception const& other) = default;

		[[nodiscard]]
		void const*
		getUnwrittenBuffer() const noexcept;

		[[nodiscard]]
		std::size_t
		getUnwrittenSize() const noexcept;

	};//class Stream::Output::Exception


	/**
	 * Move constructor
	 */
	Output(Output&& other) noexcept;

	friend void
	swap(Output& a, Output& b) noexcept;

	Output&
	operator=(Output&& other) noexcept;

	/**
	 * Link @p output to @p sink
	 */
	template <typename S, typename T>
	friend T&
	operator<(S& sink, T& output) noexcept
	requires (std::derived_from<S, Output> &&
		std::derived_from<T, Output>);

	/**
	 * Link @p output to an unwritable sink
	 */
	template <typename T>
	friend T&
	operator<(std::nullptr_t, T& output) noexcept
	requires std::derived_from<T, Output>;

	/**
	 * Call @ref flush()
	 * @throws		Output::Exception
	 */
	Output&
	operator<<(std::nullptr_t);

	/**
	 * Call @ref flush() and trigger the next output to do the same
	 * @throws		Output::Exception
	 */
	Output&
	operator<(std::nullptr_t);

	/**
	 * Write @p size bytes from @p src
	 * @param[in]	src Memory address where the data to be written will be read
	 * @param[in]	size Number of bytes to be written
	 * @return		Self-reference
	 * @pre			@p src must be a valid memory area
	 * @pre			@p size should be non-zero
	 * @throws		Output::Exception
	 */
	Output&
	write(void const* src, std::size_t size);

	/**
	 * Write @p size bytes from @p src
	 * @param[in]	src Memory address where the data to be written will be read
	 * @param[in]	size Number of bytes to be written
	 * @return		Number of bytes that can actually be written
	 * @pre			@p src must be a valid memory area
	 * @pre			@p size should be non-zero
	 * @throws		Output::Exception
	 */
	std::size_t
	writeSome(void const* src, std::size_t size);

	/**
	 * Write trivially copyable @p t
	 * @param[in]	t
	 * @return		Self-reference
	 * @throws		Output::Exception
	 */
	Output&
	operator<<(auto const& t)
	requires std::is_trivially_copyable_v<std::remove_reference_t<decltype(t)>>;

	/**
	 * Write size prepended string @p s
	 * @param[in]	s
	 * @return		Self-reference
	 * @throws		Output::Exception
	 */
	template <Char C>
	Output&
	operator<<(std::basic_string<C> const& s);

	/**
	 * Write null terminated string @p s
	 * @param[in]	s
	 * @return		Self-reference
	 * @throws		Output::Exception
	 */
	Output&
	operator<<(Char auto const* s);

};//class Stream::Output


extern Output& StdOut;
extern Output& StdErr;


std::error_code
make_error_code(Exception::Code e) noexcept;

}//namespace Stream


namespace std {

template <>
struct is_error_code_enum<Stream::Exception::Code> : true_type {};

}//namespace std


#include "../../src/InOut.tpp"

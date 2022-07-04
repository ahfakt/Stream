#ifndef STREAM_OUTPUT_H
#define STREAM_OUTPUT_H

#include <cstddef>
#include <system_error>

namespace Stream {

/**
 * @brief	Output stream base class
 * @class	Output Output.h "Stream/Output.h"
 */
class Output {
protected:
	Output() noexcept = default;

	virtual std::size_t
	writeBytes(std::byte const* src, std::size_t size) = 0;

	virtual void
	flush();

public:
	struct Exception : std::system_error {
		using std::system_error::system_error;

		enum class Code : int {
			Uninitialized = 1
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
	};//struct Exception

	Output(Output const&) = delete;

	Output&
	write(void const* src, std::size_t size);

	std::size_t
	writeSome(void const* src, std::size_t size);

	Output&
	operator<<(std::nullptr_t);
};//class Output

template <typename T>
concept Out = std::is_base_of_v<Output, T>;

class OutputFilter;

template <typename T>
concept OutFilter = std::is_base_of_v<OutputFilter, T>;

/**
 * @brief	Output stream filter base class
 * @class	OutputFilter Output.h "Stream/Output.h"
 */
class OutputFilter : public Output {
protected:
	Output* mSink;

	OutputFilter() noexcept;

	std::size_t
	writeBytes(std::byte const* src, std::size_t size) override;

public:
	friend void
	swap(OutputFilter& a, OutputFilter& b) noexcept;

	friend auto&
	operator<<(Out auto& out, OutFilter auto& outFilter) noexcept;
};//class OutputFilter

std::error_code
make_error_code(Output::Exception::Code e) noexcept;

}//namespace Stream

namespace std {

template <>
struct is_error_code_enum<Stream::Output::Exception::Code> : true_type {};

}//namespace std

#include "../src/Output.hpp"

#endif //STREAM_OUTPUT_H

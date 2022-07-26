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

	Output&
	operator<<(std::string const& str);

	Output&
	operator<<(char const* str);

	Output&
	operator<<(auto const& t)
	requires std::is_trivially_copyable_v<std::remove_reference_t<decltype(t)>>;
};//class Output

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

	friend OutputFilter&
	operator<<(Output& output, OutputFilter& outputFilter) noexcept;

	friend OutputFilter&
	operator<<(std::nullptr_t, OutputFilter& outputFilter) noexcept;
};//class OutputFilter

std::error_code
make_error_code(Output::Exception::Code e) noexcept;

}//namespace Stream

#include "../../src/Output.hpp"

#endif //STREAM_OUTPUT_H

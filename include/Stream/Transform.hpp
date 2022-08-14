#ifndef STREAM_TRANSFORM_HPP
#define STREAM_TRANSFORM_HPP

#include "Buffer.hpp"

namespace Stream {

/**
 * @brief	%Transform Input stream base class
 * @class	TransformInput Transform.hpp "Stream/Transform.hpp"
 */
class TransformInput : public Input {
	BufferInput* mSource;

protected:
	TransformInput() noexcept;

	[[nodiscard]] std::byte const*
	getData() const noexcept;

	void
	advanceData(std::size_t size) noexcept;

	std::size_t
	provideData(std::size_t min);

	std::size_t
	provideSomeData(std::size_t max);

public:
	friend void
	swap(TransformInput& a, TransformInput& b) noexcept;

	friend TransformInput&
	operator>(BufferInput& bufferInput, TransformInput& transformInput) noexcept;

	friend TransformInput&
	operator>(std::nullptr_t, TransformInput& transformInput) noexcept;
};//class Stream::TransformInput

/**
 * @brief	%Transform Output stream base class
 * @class	TransformOutput Transform.hpp "Stream/Transform.hpp"
 */
class TransformOutput : public Output {
	BufferOutput* mSink;

protected:
	TransformOutput() noexcept;

	std::byte*
	getSpace() noexcept;

	void
	advanceSpace(std::size_t size) noexcept;

	std::size_t
	provideSpace(std::size_t min);

	std::size_t
	provideSomeSpace(std::size_t max);

	void
	flush() override;

public:
	friend void
	swap(TransformOutput& a, TransformOutput& b) noexcept;

	friend TransformOutput&
	operator<(BufferOutput& bufferOutput, TransformOutput& transformOutput) noexcept;

	friend TransformOutput&
	operator<(std::nullptr_t, TransformOutput& transformOutput) noexcept;
};//class Stream::TransformOutput

}//namespace Stream

#include "../../src/Stream/Transform.tpp"

#endif //STREAM_TRANSFORM_HPP

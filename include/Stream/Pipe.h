#ifndef STREAM_PIPE_H
#define STREAM_PIPE_H

#include "InOut.h"
#include <IO/Pipe.h>

namespace Stream {

/**
 * @brief	IO::Pipe Input stream
 * @class	PipeInput Pipe.h "Stream/Pipe.h"
 */
class PipeInput : public Input {
	IO::Pipe* mPipe;

	std::size_t
	readBytes(std::byte* dest, std::size_t size) override;

public:
	struct Exception : Input::Exception
	{ using Input::Exception::Exception; };

	PipeInput() noexcept;

	PipeInput(PipeInput const&) = delete;

	PipeInput(PipeInput&& other) noexcept;

	friend void
	swap(PipeInput& a, PipeInput& b) noexcept;

	PipeInput&
	operator=(PipeInput&& other) noexcept;

	friend PipeInput&
	operator>>(IO::Pipe& pipe, PipeInput& pipeInput) noexcept;

	friend PipeInput&
	operator>>(std::nullptr_t, PipeInput& pipeInput) noexcept;
};//class PipeInput

/**
 * @brief	IO::Pipe Output stream
 * @class	PipeOutput Pipe.h "Stream/Pipe.h"
 */
class PipeOutput : public Output {
	IO::Pipe* mPipe;

	std::size_t
	writeBytes(std::byte const* src, std::size_t size) override;

public:
	struct Exception : Output::Exception
	{ using Output::Exception::Exception; };

	PipeOutput() noexcept;

	PipeOutput(PipeOutput const&) = delete;

	PipeOutput(PipeOutput&& other) noexcept;

	friend void
	swap(PipeOutput& a, PipeOutput& b) noexcept;

	PipeOutput&
	operator=(PipeOutput&& other) noexcept;

	friend PipeOutput&
	operator<<(IO::Pipe& pipe, PipeOutput& pipeOutput) noexcept;

	friend PipeOutput&
	operator<<(std::nullptr_t, PipeOutput& pipeOutput) noexcept;
};//class PipeOutput

/**
 * @brief IO::Pipe Input / Output stream
 * @class Pipe Pipe.h "Stream/Pipe.h"
 */
class Pipe : public PipeInput, public PipeOutput {};

void
swap(Pipe& a, Pipe& b) noexcept;

template <typename T>
concept PipeInOut = std::derived_from<T, PipeInput> && std::derived_from<T, PipeOutput>;

auto&
operator<=>(IO::Pipe& pipe, PipeInOut auto& pipeInOut) noexcept
{
	pipe >> pipeInOut;
	pipe << pipeInOut;
	return pipeInOut;
}

auto&
operator<=>(std::nullptr_t, PipeInOut auto& pipeInOut) noexcept
{
	nullptr >> pipeInOut;
	nullptr << pipeInOut;
	return pipeInOut;
}

}//namespace Stream

#endif //STREAM_PIPE_H

#pragma once

#include "InOut.hpp"

namespace Stream {

/**
 * @brief	%Transparent Input stream base class
 * @class	TransparentInput Transparent.hpp "Stream/Transparent.hpp"
 */
class TransparentInput : public Input {
	Input* mSource;

protected:
	TransparentInput() noexcept;

	std::size_t
	readBytes(std::byte* dest, std::size_t size) override;

	std::size_t
	getSome(void* dest, std::size_t size);

public:
	TransparentInput(TransparentInput&& other) noexcept;

	friend void
	swap(TransparentInput& a, TransparentInput& b) noexcept;

	TransparentInput&
	operator=(TransparentInput&& other) noexcept;

	friend TransparentInput&
	operator>(Input& input, TransparentInput& transparentInput) noexcept;

	friend TransparentInput&
	operator>(std::nullptr_t, TransparentInput& transparentInput) noexcept;
};//class Stream::TransparentInput

/**
 * @brief	%Transparent Output stream base class
 * @class	TransparentOutput Transparent.hpp "Stream/Transparent.hpp"
 */
class TransparentOutput : public Output {
	Output* mSink;

protected:
	TransparentOutput() noexcept;

	std::size_t
	writeBytes(std::byte const* src, std::size_t size) override;

	std::size_t
	putSome(void const* src, std::size_t size);

public:
	TransparentOutput(TransparentOutput&& other) noexcept;

	friend void
	swap(TransparentOutput& a, TransparentOutput& b) noexcept;

	TransparentOutput&
	operator=(TransparentOutput&& other) noexcept;

	friend TransparentOutput&
	operator<(Output& output, TransparentOutput& transparentOutput) noexcept;

	friend TransparentOutput&
	operator<(std::nullptr_t, TransparentOutput& transparentOutput) noexcept;
};//class Stream::TransparentOutput

}//namespace Stream

#include "../../src/Stream/Transparent.tpp"

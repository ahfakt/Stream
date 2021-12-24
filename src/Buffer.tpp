#pragma once

#include "Stream/Buffer.hpp"


namespace Stream {

template <typename S, typename T>
T&
operator>(S& bufferSource, T& bufferReader) noexcept
requires (std::derived_from<S, Input> && std::derived_from<S, BufferInput> &&
	std::derived_from<T, Input> && std::derived_from<T, BufferReader>)
{
	bufferSource > static_cast<Input&>(bufferReader);
	return bufferReader;
}

template <typename T>
T&
operator>(std::nullptr_t, T& bufferReader) noexcept
requires (std::derived_from<T, Input> && std::derived_from<T, BufferReader>)
{ return *BufferReader::Unreadable > bufferReader; }


template <typename S, typename T>
T&
operator<(S& bufferSink, T& bufferWriter) noexcept
requires (std::derived_from<S, Output> && std::derived_from<S, BufferOutput> &&
	std::derived_from<T, Output> && std::derived_from<T, BufferWriter>)
{
	bufferSink < static_cast<Output&>(bufferWriter);
	return bufferWriter;
}

template <typename T>
T&
operator<(std::nullptr_t, T& bufferWriter) noexcept
requires (std::derived_from<T, Output> && std::derived_from<T, BufferWriter>)
{ return *BufferWriter::Unwritable < bufferWriter; }


/**
 * Link @p bufferReaderWriter to @p bufferInputOutput for input and output
 */
template <typename S, typename T>
T&
operator|(S& bufferInputOutput, S& bufferReaderWriter) noexcept
requires (std::derived_from<S, Input> && std::derived_from<S, BufferInput> && std::derived_from<S, Output> && std::derived_from<S, BufferOutput> &&
	std::derived_from<T, Input> && std::derived_from<T, BufferReader> && std::derived_from<T, Output> && std::derived_from<T, BufferWriter>)
{
	static_cast<BufferInput& >(bufferInputOutput) > static_cast<BufferReader&>(bufferReaderWriter);
	static_cast<BufferOutput&>(bufferInputOutput) < static_cast<BufferWriter&>(bufferReaderWriter);
	return bufferReaderWriter;
}

/**
 * Disallow linking @p bufferReaderWriter to @p sourceSink
 */
template <typename S, typename T>
T&
operator|(S& sourceSink, T& bufferReaderWriter) noexcept
requires (std::derived_from<S, Input> && !std::derived_from<S, BufferInput> && std::derived_from<S, Output> && !std::derived_from<S, BufferOutput> &&
	std::derived_from<T, Input> && std::derived_from<T, BufferReader> && std::derived_from<T, Output> && std::derived_from<T, BufferWriter>
) = delete/*("BufferReaderWriter needs a buffer and can not link to a sourceSink directly")*/;

/**
 * Link @p bufferReaderWriter to an unreadable buffer source and to an unwritable buffer sink
 */
template <typename T>
T&
operator|(std::nullptr_t, T& bufferReaderWriter) noexcept
requires (std::derived_from<T, Input> && std::derived_from<T, BufferReader> && std::derived_from<T, Output> && std::derived_from<T, BufferWriter>)
{
	nullptr > static_cast<BufferReader&>(bufferReaderWriter);
	nullptr < static_cast<BufferWriter&>(bufferReaderWriter);
	return bufferReaderWriter;
}

}//namespace Stream

/**
 * @pre	@p s must be non-zero
 * @pre @p divider must be non-zero
 */
inline std::size_t
ceilz(std::size_t s, std::size_t divider) noexcept
{ return (s - 1) / divider + 1; }
	
inline std::size_t
floorz(std::size_t s, std::size_t divider) noexcept
{ return s / divider; }

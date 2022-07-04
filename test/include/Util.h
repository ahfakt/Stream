#ifndef STREAMTEST_UTIL_H
#define STREAMTEST_UTIL_H

#include <memory>
#include <random>
#include <chrono>
#include <vector>
#include "Stream/InOut.h"

namespace StreamTest::Util {

template <typename duration>
std::vector<std::byte>
GetRandomBytes(std::size_t n) {
	std::default_random_engine engine(
			std::chrono::duration_cast<duration>(
					std::chrono::system_clock::now().time_since_epoch()
			).count()
	);
	std::uniform_int_distribution<std::uint8_t> distribution(0, 255);
	std::vector<std::byte> v;
	v.reserve(n);
	for (std::size_t i = 0; i < n; ++i)
		v.push_back(static_cast<std::byte>(distribution(engine)));
	return v;
}

void
WriteRandomChunks(Stream::Output& output, std::vector<std::byte> const& input, std::uniform_int_distribution<int> distribution)
{
	std::random_device rd;
	std::mt19937 gen(rd());

	for (std::size_t total = 0, r, size = input.size(); total < size; total += r) {
		r = distribution(gen);
		if (total + r > size)
			r = size - total;
		output.write(input.data() + total, r);
	}
}

void
ReadRandomChunks(Stream::Input& input, std::vector<std::byte>& output, std::uniform_int_distribution<int> distribution)
{
	std::random_device rd;
	std::mt19937 gen(rd());

	for (std::size_t total = 0, r, size = output.size(); total < size; total += r) {
		r = distribution(gen);
		if (total + r > size)
			r = size - total;
		input.read(output.data() + total, r);
	}
}

std::vector<std::byte>
ReadFile(std::string const& fileName);

void
WriteFile(std::string const& fileName, void* src, std::size_t size)
{
	IO::File file(fileName, IO::File::Mode::W);
	Stream::FileOutput fileOutput;
	file << fileOutput;
	fileOutput.write(src, size);
}

void
AppendFile(std::string const& fileName, void* src, std::size_t size)
{
	IO::File file(fileName, IO::File::Mode::A);
	Stream::FileOutput fileOutput;
	file << fileOutput;
	fileOutput.write(src, size);
}

}//namespace StreamTest::Util

#endif //STREAMTEST_UTIL_H
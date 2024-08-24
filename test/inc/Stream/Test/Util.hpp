#pragma once

#include "Stream/InOut.hpp"
#include <memory>
#include <random>
#include <chrono>
#include <vector>

namespace Stream::Test {

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
	for (std::size_t i{0}; i < n; ++i)
		v.push_back(static_cast<std::byte>(distribution(engine)));
	return v;
}

void
WriteRandomChunks(Stream::Output& output, std::vector<std::byte> const& input, std::uniform_int_distribution<int> distribution);

void
ReadRandomChunks(Stream::Input& input, std::vector<std::byte>& output, std::uniform_int_distribution<int> distribution);

}//namespace Stream::Test

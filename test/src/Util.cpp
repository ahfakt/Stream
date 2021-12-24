#include "Stream/Test/Util.hpp"

namespace Stream::Test {

void
WriteRandomChunks(Stream::Output& output, std::vector<std::byte> const& input, std::uniform_int_distribution<int> distribution)
{
	std::random_device rd;
	std::mt19937 gen(rd());

	for (std::size_t total{0}, r, size = input.size(); total < size; total += r) {
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

	for (std::size_t total{0}, r, size = output.size(); total < size; total += r) {
		r = distribution(gen);
		if (total + r > size)
			r = size - total;
		input.read(output.data() + total, r);
	}
}

}//namespace Stream::Test

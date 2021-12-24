#include <Stream/Text.hpp>
#include <cassert>

int main()
{
	using namespace std::string_view_literals;
	auto sv{
		"\n"
		"\r\n"
		"Line\n"
		"Line\r\n"
		"\r"sv
	};
	Stream::BufferInput buffer(sv.begin(), sv.size());
	Stream::TextInput str;

	buffer > str;

	assert(str.getLine().empty());
	assert(str.getLine().empty());
	assert(str.getLine() == "Line");
	assert(str.getLine() == "Line");
	assert(str.getLine() == "\r");

	try {
		str.getLine();
		assert(false);
	} catch (Stream::Input::Exception const& exc) {
		assert((exc.code() == std::make_error_code(std::errc::no_message_available)));
	}

	return 0;
}


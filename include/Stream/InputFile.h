#ifndef STREAM_INPUT_FILE_H
#define STREAM_INPUT_FILE_H

#include "Input.h"

namespace Stream {

class InputFile:
public Input {
	std::align_val_t mAlignment;
	std::size_t mBufferSize;
	int mFileDescriptor;

	bool load() override;
public:
	InputFile(char const* fileName);
	~InputFile();
};//class InputFile

}//namespace Stream

#endif //STREAM_INPUT_FILE_H

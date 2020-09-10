#ifndef STREAM_OUTPUT_FILE_H
#define STREAM_OUTPUT_FILE_H

#include "Output.h"

namespace Stream {

class OutputFile:
public Output {
	std::align_val_t mAlignment;
	std::size_t mBufferSize;
	int mFileDescriptor;

	bool store() override;
	bool incompleteWrite();
public:
	OutputFile(char const* fileName, bool append = false);
	~OutputFile();
};//class OutputFile

}//namespace Stream

#endif //STREAM_OUTPUT_FILE_H

#include "Stream/InputFile.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <cstdio>
#include <unistd.h>

namespace Stream {

InputFile::InputFile(char const* fileName):
	mFileDescriptor(open(fileName, O_RDONLY|O_DIRECT)),
	mAlignment(std::align_val_t{4096}), mBufferSize(128*1024) {
	if (mFileDescriptor > 2) { // stdin stdout stderr
		struct stat fileStatus;
		if (fstat(mFileDescriptor, &fileStatus) < 0) {
			err = true;
			perror("fstat()");
			return;
		}
		if (mBufferSize < fileStatus.st_blksize)
			mBufferSize = fileStatus.st_blksize;
		else {
			if (fileStatus.st_size <= fileStatus.st_blksize)
				mBufferSize = fileStatus.st_blksize;
			else if(fileStatus.st_size < mBufferSize) // ceil
				mBufferSize = (fileStatus.st_size + fileStatus.st_blksize - 1) & ~(fileStatus.st_blksize - 1);
		}

		mGetBegin = reinterpret_cast<std::byte*>(::operator new(mBufferSize, mAlignment = std::align_val_t{fileStatus.st_blksize}));
		mGetEnd = mGetCurrent = mGetBegin + mBufferSize;
	}
	else {
		err = true;
		perror("open()");
	}
}

InputFile::~InputFile() {
	::operator delete (mGetBegin, mAlignment);
	if (mFileDescriptor > 2 && close(mFileDescriptor) < 0) perror("close()");
}

bool
InputFile::load() {
	ssize_t res = read(mFileDescriptor, mGetBegin, mBufferSize);
	if (res <= 0) {
		if (res < 0) {
			err = true;
			perror("read()");
		}
		eof = true;
		return true;
	}
	// TODO: if (res < mBufferSize)
	mGetEnd = (mGetCurrent = mGetBegin) + res;
	return false;
}

}//namespace Stream

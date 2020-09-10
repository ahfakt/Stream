#include "Stream/OutputFile.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <cstdio>
#include <unistd.h>

namespace Stream {

OutputFile::OutputFile(char const* fileName, bool append):
	mFileDescriptor(open(fileName, (append ? O_APPEND : O_TRUNC)|O_CREAT|O_WRONLY|O_DIRECT|O_SYNC, S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH)),
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

		mPutBegin = reinterpret_cast<std::byte*>(::operator new(mBufferSize, mAlignment = std::align_val_t{fileStatus.st_blksize}));
		mPutEnd = (mPutCurrent = mPutBegin) + mBufferSize;
	}
	else {
		err = true;
		perror("open()");
	}
}

OutputFile::~OutputFile() {
	mPutCurrent == mPutEnd ? store() : incompleteWrite();
	::operator delete (mPutBegin, mAlignment);
	if (mFileDescriptor > 2 && close(mFileDescriptor) < 0) perror("close()");
}

bool
OutputFile::store() {
	ssize_t res = write(mFileDescriptor, mPutBegin, mBufferSize);
	if (res < 0) {
		err = true;
		perror("write()");
		return true;
	}
	// TODO: if (res < mBufferSize)
	mPutCurrent = mPutBegin;
	return false;
}

bool OutputFile::incompleteWrite() {
	struct stat fileStatus;
	if (fstat(mFileDescriptor, &fileStatus) < 0) {
		err = true;
		perror("fstat()");
		return true;
	}

	std::size_t lastBufferSize = mPutCurrent - mPutBegin;
	ssize_t res = write(mFileDescriptor, mPutBegin, // ceil
		(lastBufferSize + fileStatus.st_blksize - 1) & ~(fileStatus.st_blksize - 1));
	if (res < 0) {
		err = true;
		perror("write()");
		return true;
	}
	// TODO: if (res < lastBufferSize)
	if (ftruncate(mFileDescriptor, fileStatus.st_size + lastBufferSize) < 0) {
		err = true;
		perror("ftruncate()");
		return true;
	}
	//mPutCurrent = mPutBegin; destructor is only caller
	return false;
}

}//namespace Stream

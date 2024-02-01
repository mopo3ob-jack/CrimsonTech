#include "../../include/file/MappedFile.hpp"
#include "../../include/utils/hdbError.hpp"

#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <string.h>
#include <errno.h>

namespace hdb {

namespace ct {

MappedFile::MappedFile() {}

MappedFile::MappedFile(const char* filepath, unsigned int flags, unsigned int prot, unsigned int permissions) {
	openFile(filepath, flags, prot, permissions);
}

MappedFile::~MappedFile() {
	closeFile();
}

void MappedFile::openFile(const char* filepath, unsigned int flags, unsigned int prot, unsigned int permissions) {
	closeFile();

	if (permissions)
		fd = open(filepath, flags, permissions);
	else
		fd = open(filepath, flags);

	if (fd == -1) {
		error("Openning file: %s failed with: %s\n", filepath, strerror(errno));
	}

	mmapped = mmap(nullptr, lseek(fd, 0, SEEK_END), prot, MAP_SHARED, fd, 0);
}

void MappedFile::closeFile() {
	if (fd != -1) {
		munmap(mmapped, lseek(fd, 0, SEEK_END));
		close(fd);
		fd = -1;
	}
}

}

}
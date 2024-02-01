#ifndef HDB_CT_MAPPEDFILE_HPP
#define HDB_CT_MAPPEDFILE_HPP

#include <unistd.h>
#include <sys/mman.h>

namespace hdb {

namespace ct {

class MappedFile {
public:
	MappedFile();

	MappedFile(const char* filepath, unsigned int flags, unsigned int prot, unsigned int permissions = 0);

	~MappedFile();

	MappedFile(const MappedFile& file) = delete;
	MappedFile operator=(const MappedFile& file) = delete;

	void openFile(const char* filepath, unsigned int flags, unsigned int prot, unsigned int permissions = 0);
	void closeFile();

	unsigned long fileLength() const { return lseek(fd, 0, SEEK_END); }

	template <typename T>
	operator T*() {
		return (T*)mmapped;
	}

private:
	void* mmapped;
	unsigned int fd = -1;
};

}

}

#endif
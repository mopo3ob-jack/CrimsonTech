#ifndef HDB_ALLOC_H
#define HDB_ALLOC_H

#include <stdlib.h>

#ifdef __cplusplus

namespace hdb {

template <typename T>
static inline T* alloc(size_t size, size_t alignment = alignof(T)) {
	return (T*)aligned_alloc(alignment, size * sizeof(T));
}

template <typename T>
static inline T* alloc(T*& ptr, size_t size, size_t alignment = alignof(T)) {
	return ptr = (T*)aligned_alloc(alignment, size * sizeof(T));
}

template <typename T>
static inline void resize(T*& ptr, size_t size) {
	ptr = (T*)realloc(ptr, size * sizeof(T));
}

template <typename T>
static inline void free(T* ptr) {
	std::free((void*)ptr);
}

}

#endif

#endif
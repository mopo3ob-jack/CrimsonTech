#ifndef HDB_ERROR_HPP
#define HDB_ERROR_HPP

#include <stdio.h>

#define hdbError(fmt, ...) hdb::__error(__FILE__, __LINE__, fmt, ##__VA_ARGS__, 0)

namespace hdb {

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-result"
#pragma GCC diagnostic ignored "-Wformat-security"
template <typename... Args>
static inline int __error(const char* file, unsigned int line, const char* fmt, Args... args) {
	fprintf(stderr, "\033[31mError: \033[0mIn file: %s:%d\n", file, line);
	return fprintf(stderr, fmt, args...);
}

template <typename... Args>
static inline int error(const char* fmt, Args... args) {
	fprintf(stderr, "\033[31mError: \033[0m");
	return fprintf(stderr, fmt, args...);
}

#pragma GCC diagnostic pop

#ifdef __gl_h_

#define checkGLError() __checkGLError(__FILE__, __LINE__)

namespace ct {

static inline void __checkGLError(const char* file, unsigned int line) {
	unsigned int errorCode = glGetError();
	if (errorCode) {
		switch (errorCode) {
		case GL_INVALID_ENUM: __error(file, line, "GL_INVALID_ENUM\n"); return;
		case GL_INVALID_VALUE: __error(file, line, "GL_INVALID_VALUE\n"); return;
		case GL_INVALID_OPERATION: __error(file, line, "GL_INVALID_OPERATION\n"); return;
		// case GL_STACK_OVERFLOW: __error(file, line, "GL_STACK_OVERFLOW\n"); return;
		// case GL_STACK_UNDERFLOW: __error(file, line, "GL_INVALID_UNDERFLOW\n"); return;
		case GL_OUT_OF_MEMORY: __error(file, line, "GL_OUT_OF_MEMORY\n"); return;
		case GL_INVALID_FRAMEBUFFER_OPERATION:  __error(file, line, "GL_INVALID_FRAMEBUFFER_OPERATION\n"); return;
		default: __error(file, line, "UNKNOWN GL ERROR\n");
		}
	}
}

}

#endif

}

#endif
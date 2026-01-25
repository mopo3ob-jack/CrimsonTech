#ifndef CT_STORAGEBUFFER_HPP
#define CT_STORAGEBUFFER_HPP

#include <glad/glad.h>
#include <GL/glu.h>
#include <mstd/memory>

namespace ct {

class StorageBuffer {
public:
	StorageBuffer() : handle(0) {}

	template <typename T>
	StorageBuffer(T* data, GLsizeiptr size) {
		glCreateBuffers(1, &handle);

		glNamedBufferStorage(handle, sizeof(T) * size, data, GL_DYNAMIC_STORAGE_BIT);
	}

	StorageBuffer(StorageBuffer& sb) = delete;
	StorageBuffer& operator=(StorageBuffer& sb) = delete;

	StorageBuffer(StorageBuffer&& sb) {
		*this = std::move(sb);
	}

	StorageBuffer& operator=(StorageBuffer&& sb) {
		this->handle = sb.handle;

		sb.handle = 0;

		return *this;
	}

	~StorageBuffer() {
		if (handle) {
			glDeleteBuffers(1, &handle);
			handle = 0;
		}
	}

	template <typename T>
	void write(T* data, GLsizeiptr count, GLintptr offset = 0L) {
		glNamedBufferSubData(handle, offset * sizeof(T), count * sizeof(T), (void*)data);
	}

	operator GLuint() const {
		return handle;
	}

	void bind(GLuint index) const {
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, index, handle);
	}

private:
	GLuint handle;
};

}

#endif

#ifndef CRIMSON_RENDERER_GL_HPP
#define CRIMSON_RENDERER_GL_HPP

#ifdef _glfw3_h_
#error GAH
#endif

#ifdef _WIN32
	#ifndef WIN32_LEAN_AND_MEAN
		#define WIN32_LEAN_AND_MEAN
	#endif
	#include <windows.h>
	#include <windef.h>
#endif

#include <glad/glad.h>
#include <gl/GLU.h>

#endif
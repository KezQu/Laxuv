#pragma once
#include <GL/glew.h>
#include <iostream>
#include <Log.h>

#define _(x) x; \
GLenum err = glGetError(); \
if(err != GL_NO_ERROR) \
{ Log() << "Error " << err << " in file : " << __FILE__ << " line : " << __LINE__ <<std::endl; \
__debugbreak();}

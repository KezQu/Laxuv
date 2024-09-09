#pragma once

#include <GL/glew.h>
#include <iostream>
#include <sstream>
#include <chrono>
#include <deque>
#include <regex>

template<typename T>
concept is_streamable = requires(std::ostringstream stream, T value) {
	stream << value << std::endl;
};

class console {
private:
	std::ostringstream log;
public:
	operator std::ostringstream& () {
		return log;
	}
	template<is_streamable T>
	std::ostringstream& operator<<(T const& LogMessage)
	{
		auto timestamp = std::chrono::system_clock().now();
		 log << "[" << std::format("{:%T}", std::chrono::floor<std::chrono::seconds>(timestamp)) << "] " << LogMessage;
		return log;
	}
};

extern console LOG;

#define _(x) x; \
{ GLenum err = glGetError(); \
if(err != GL_NO_ERROR) \
{ LOG << "Error " << std::hex << err << " in file : " << __FILE__ << " line : " << __LINE__ <<std::endl;\
std::cout << "Error " << std::hex << err << " in file : " << __FILE__ << " line : " << __LINE__ <<std::endl;\
 __debugbreak();\
}}

#pragma once

#include <chrono>
#include <iostream>

#include "GL/glew.h"

template <typename T>
concept is_streamable = requires(std::ostringstream stream, T value) {
  stream << value << std::endl;
};

class console
{
 private:
  std::ostringstream log;

 public:
  // Create conversion formula to allow easier logging mechanism
  operator std::ostringstream&()
  {
    return log;
  }
  // Create a log message out of received message and store it in console cache
  template <is_streamable T>
  std::ostringstream& operator<<(T const& LogMessage)
  {
    auto timestamp = std::chrono::system_clock().now();
    log << "["
        << std::format("{:%T}",
                       std::chrono::floor<std::chrono::seconds>(timestamp))
        << "] " << LogMessage;
    return log;
  }
};

extern console LOG;

// Macro for easier debugging OpenGL runtime errors
#define _(x)                                                                \
  x;                                                                        \
  {                                                                         \
    GLenum err = glGetError();                                              \
    if (err != GL_NO_ERROR)                                                 \
    {                                                                       \
      LOG << "Error " << std::hex << err << " in file : " << __FILE__       \
          << " line : " << __LINE__ << std::endl;                           \
      std::cout << "Error " << std::hex << err << " in file : " << __FILE__ \
                << " line : " << __LINE__ << std::endl;                     \
    }                                                                       \
  }

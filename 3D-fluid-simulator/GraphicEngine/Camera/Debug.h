#pragma once

#include <chrono>
#include <iomanip>
#include <iostream>

#include "GL/glew.h"

template <typename T>
concept is_streamable = requires(std::ostringstream stream, T value) {
  stream << value << std::endl;
};

/**
 * @brief helper class that represents a console with cached logs with messages
 * and errors that occurred during runtime
 *
 */
class console
{
 private:
  std::ostringstream log;

 public:
  /**
   * @brief Create conversion formula to allow easier logging mechanism
   *
   * @return std::ostringstream&
   */
  operator std::ostringstream&()
  {
    return log;
  }
  /**
   * @brief Create a log message out of received message and store it in console
   * cache
   *
   * @param LogMessage message to be displayed in the console
   * @return template <is_streamable T>&
   */
  template <is_streamable T>
  std::ostringstream& operator<<(T const& LogMessage)
  {
    auto const timestamp = RetrieveTimestamp();
    log << timestamp << LogMessage;
    return log;
  }

  std::ostringstream& operator<<(char const* const LogMessage)
  {
    auto const timestamp = RetrieveTimestamp();
    log << timestamp << LogMessage;
    return log;
  }
  /**
   * @brief Helper method to create a timestamp for a log
   *
   * @return std::string
   */
  std::string RetrieveTimestamp()
  {
    auto const timestamp = std::chrono::system_clock().now().time_since_epoch();
    auto const hours =
        std::chrono::duration_cast<std::chrono::hours>(timestamp) -
        std::chrono::duration_cast<std::chrono::days>(timestamp);
    auto const minutes =
        std::chrono::duration_cast<std::chrono::minutes>(timestamp) -
        std::chrono::duration_cast<std::chrono::hours>(timestamp);
    auto const seconds =
        std::chrono::duration_cast<std::chrono::seconds>(timestamp) -
        std::chrono::duration_cast<std::chrono::minutes>(timestamp);
    return std::string{"["} + std::to_string(hours.count()) + ":" +
           std::to_string(minutes.count()) + ":" +
           std::to_string(seconds.count()) + "] ";
  }
};

extern console LOG;

// Macro for easier debugging OpenGL runtime errors
#define _(x)                                                          \
  x;                                                                  \
  {                                                                   \
    GLenum err = glGetError();                                        \
    if (err != GL_NO_ERROR)                                           \
    {                                                                 \
      LOG << "Error " << std::hex << err << " in file : " << __FILE__ \
          << " line : " << __LINE__ << std::endl;                     \
    }                                                                 \
  }

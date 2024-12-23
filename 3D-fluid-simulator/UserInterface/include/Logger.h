#pragma once

#include <deque>
#include <sstream>

#include "Interface.h"

/**
 * @brief Class implementing 'Interface' interface, responsible for displaying
 * messages retrieved from OpenGL context and additional runtime errors
 *
 */
class Logger : public Interface
{
 private:
  static std::deque<std::string> _logHistory;

 public:
  /**
   * @brief Construct a new Logger object with a specified properties
   *
   * @param log Stream containing data to be displayed to the user
   * @param size Size of the Logger window
   * @param position Relative position of the logger inside a main window
   */
  Logger(std::ostringstream& log, ImVec2 const& size, ImVec2 const& position);
  /**
   * @brief Method used for specifying contents of the logger
   *
   */
  void Generate() override;
};
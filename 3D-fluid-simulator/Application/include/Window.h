#pragma once
#define GLFW_INCLUDE_NONE

#include <iostream>

#include "Cube.h"
#include "Explorer.h"
#include "GLFW/glfw3.h"
#include "Line.h"
#include "Logger.h"
#include "Object.h"
#include "Particles.h"
#include "PhysicsDispatch.h"
#include "Point.h"
#include "Sphere.h"
#include "Square.h"
#include "Toolbar.h"
#include "WorldAxes.h"

/**
 * @brief Class representing the main window of a simulation
 *
 */
class Window
{
 public:
  /**
   * @brief Constructor that allows creation a a main simulation window
   *
   * @param windowSize Initial window size
   * @param windowTitle Window title
   */
  Window(ImVec2 const& windowSize, std::string const& windowTitle);
  /**
   * @brief Allows safe deletion of a window and contained contexts
   *
   */
  ~Window();
  /**
   * @brief Main simulation loop responsible for drawing and calculating CFD
   *
   */
  void EventLoop();
  /**
   * @brief Method that is responsible for clearing current window frame
   *
   */
  void Refresh();
  /**
   * @brief Method that allows to render created frame
   *
   */
  void Render();
  /**
   * @brief Method that cache current window size
   *
   */
  void FramebufferResizeCheck();
  /**
   * @brief Method that is responsible for processing inputs from mouse and
   * keyboard
   *
   */
  void ProcessKeyInputs();
  /**
   * @brief Function that allows to retrieve errors from GLFW library
   *
   * @param error Error code
   * @param description Description of a returned error
   */
  static void GLFWErrorCallback(int error, const char* description);
  /**
   * @brief Funkcja pozwalająca na zwrócenie błędu z interfejsu OpenGL
   * @brief Function that allows to retrieve error code from OpenGL interface
   *
   * @param source source od a error
   * @param type error type
   * @param id error id
   * @param severity severity of a error that occurred
   * @param length length if a error message
   * @param message erro message
   * @param userParam
   */
  static void OpenGLErrorCallback(GLenum source, GLenum type, GLuint id,
                                  GLenum severity, GLsizei length,
                                  const GLchar* message, const void* userParam);
  /**
   * @brief Function registered to retrieve window size when changed
   *
   * @param window window whose size changed
   * @param width new width window
   * @param height new height window
   */
  static void GLFWFrameBufferResizeCallback(GLFWwindow* window, int width,
                                            int height);
  /**
   * @brief Allows to process user input related to main window
   *
   * @param window main window
   * @param key key value whose change was registered
   * @param scancode unique key value dependent on the platform
   * @param action Action type, for example pressed, hold, released
   * @param mods
   */
  static void GLFWKeyCallback(GLFWwindow* window, int key, int scancode,
                              int action, int mods);

 private:
  /**
   * @brief Helper function that is responsible for calculating size of a
   * simulation window
   *
   * @param windowSize window size
   * @return ImVec4 Coordinates that specify the starting and end point of a
   * OpenGL context relative to main window
   */
  static ImVec4 CalculateViewport(ImVec2 const& windowSize);

  std::string _windowTitle;
  ImVec2 _windowSize;
  GLFWwindow* _window;
};

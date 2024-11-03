#pragma once
#define GLFW_INCLUDE_NONE

#include <iostream>

#include "Explorer.h"
#include "GLFW/glfw3.h"
#include "Line.h"
#include "Logger.h"
#include "Object.h"
#include "Particles.h"
#include "PhysicsDispatch.h"
#include "Point.h"
#include "Cube.h"
#include "Sphere.h"
#include "Square.h"
#include "Toolbar.h"
#include "WorldAxes.h"

class Window
{
 private:
  std::string _windowTitle;
  ImVec2 _windowSize;
  GLFWwindow* _window;

 public:
  Window(ImVec2 const& windowSize, std::string const& windowTitle);
  ~Window();
  void EventLoop();
  void Refresh();
  void Render();
  void FramebufferResizeCheck();
  void ProcessKeyInputs();
  void LoadFont(std::vector<std::string> const& font_paths);
  static void GLFWErrorCallback(int error, const char* description);
  static void OpenGLErrorCallback(GLenum source, GLenum type, GLuint id,
                                  GLenum severity, GLsizei length,
                                  const GLchar* message, const void* userParam);
  static void GLFWFrameBufferResizeCallback(GLFWwindow* window, int width,
                                            int height);
  static void GLFWKeyCallback(GLFWwindow* window, int key, int scancode,
                              int action, int mods);

 private:
  static ImVec4 CalculateViewport(ImVec2 const& windowSize);
};

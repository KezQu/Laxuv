#pragma once

#include <iostream>

#include <Qube.h>
#include <Sphere.h>
#include <Square.h>
#include <WorldAxes.h>
#include <Toolbar.h>
#include <Explorer.h>
#include <Log.h>
#include <GLFW/glfw3.h>

class Window{
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
	static void GLFWErrorCallback(int error, const char* description);
	static void OpenGLErrorCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam);
	static void GLFWFrameBufferResizeCallback(GLFWwindow* window, int width, int height);
	static void GLFWKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
private:
	static ImVec4 CalculateViewport(ImVec2 const& windowSize);
};

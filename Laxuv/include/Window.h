#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>

#include <Toolbar.h>
#include <Explorer.h>
#include <Log.h>
#include <IndexBuffer.h>

class Window{
private:
	std::pair<int, int> _windowSize;
	std::string _windowTitle;
	GLFWwindow* _window;
public:
	Window(std::pair<int, int> const& windowSize, std::string const& windowTitle);
	~Window();
	void EventLoop();
	void Refresh();
	void Render();
	void FramebufferResizeCheck();
	static void GLFWErrorCallback(int error, const char* description);
	static void GLFWFrameBufferResizeCallback(GLFWwindow* window, int width, int height);
	static void GLFWKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
};

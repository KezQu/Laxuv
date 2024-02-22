#include "Window.h"

Window::Window(std::pair<int, int> const& windowSize, std::string const& windowTitle)
	:_windowSize{windowSize},
	_windowTitle{windowTitle}
{
	glfwSetErrorCallback(Window::GLFWErrorCallback);
	if (glfwInit() == GLFW_FALSE) {
		std::cerr << "Unable to initialize GLFW context" << std::endl;
		throw std::runtime_error("Unable to initialize GLFW context");
	}
	else {
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
		glfwWindowHint(GLFW_REFRESH_RATE, 60);
		glfwSwapInterval(1);

		_window = glfwCreateWindow(_windowSize.first, _windowSize.second, _windowTitle.c_str(), glfwGetPrimaryMonitor(), nullptr);
		if (_window == nullptr) {
			std::cerr << "Unable to create window" << std::endl;
			throw std::runtime_error("Unable to create window");
		}
		glfwMakeContextCurrent(_window);

		GLenum errorCode = glewInit();
		if (errorCode != GLEW_OK) {
			std::cerr << "Unable to initialize OpenGL glewInit failed" << std::endl;
			throw std::runtime_error((const char*)(glewGetErrorString(errorCode)));
		}
		glfwSetFramebufferSizeCallback(_window, Window::GLFWFrameBufferResizeCallback);
		glfwSetKeyCallback(_window, Window::GLFWKeyCallback);
	}
}

Window::~Window()
{
	glfwDestroyWindow(_window);
	glfwTerminate();
}
void Window::EventLoop() {
	while (glfwWindowShouldClose(_window) == GLFW_FALSE) {
		Refresh();
	}
}
void Window::Refresh() {
	glfwPollEvents();
	glfwSwapBuffers(_window);
}

void Window::GLFWErrorCallback(int error, const char* description){
	std::cerr << "Error " << error << ": " << description << std::endl;
}
void Window::GLFWFrameBufferResizeCallback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
}
void Window::GLFWKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, GLFW_TRUE);
	}
}

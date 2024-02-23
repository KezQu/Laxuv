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

		_window = glfwCreateWindow(_windowSize.first, _windowSize.second, _windowTitle.c_str(), glfwGetPrimaryMonitor(), nullptr);
		if (_window == nullptr) {
			std::cerr << "Unable to create window" << std::endl;
			throw std::runtime_error("Unable to create window");
		}
		glfwMakeContextCurrent(_window);
		glfwSwapInterval(1);

		GLenum errorCode = glewInit();
		if (errorCode != GLEW_OK) {
			std::cerr << "Unable to initialize OpenGL glewInit failed" << std::endl;
			throw std::runtime_error((const char*)(glewGetErrorString(errorCode)));
		}
		glfwSetFramebufferSizeCallback(_window, Window::GLFWFrameBufferResizeCallback);
		glfwSetKeyCallback(_window, Window::GLFWKeyCallback);

		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO();
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // IF using Docking Branch
		
		// Setup Platform/Renderer backends
		ImGui_ImplGlfw_InitForOpenGL(_window, true);          // Second param install_callback=true will install GLFW callbacks and chain to existing ones.
		ImGui_ImplOpenGL3_Init();
	}
}

Window::~Window()
{
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
	glfwDestroyWindow(_window);
	glfwTerminate();
}
void Window::EventLoop() {
	Log() << "Performs basic arithmetic operations between two durations or between a duration and a tick count.\
\
		1) Converts the two durations to their common type and creates a duration whose tick count is the sum of the tick counts after conversion.\
		2) Converts the two durations to their common type and creates a duration whose tick count is the rhs number of ticks subtracted from the lhs number of ticks after conversion.";
	while (glfwWindowShouldClose(_window) == GLFW_FALSE) {
		Refresh();
		ImGui::ShowDemoWindow(); // Show demo window! :)
		Toolbar(ImVec2{ (float)(_windowSize.first),20 }, ImVec2{ 0,0 }).Generate();
		Explorer(ImVec2{ _windowSize.first / 4.f,(float)(_windowSize.first) - 20 }, ImVec2{ 0,20 }).Generate();
		Log(ImVec2{ 3 * _windowSize.first / 4.f, 200 }, ImVec2{ _windowSize.first / 4.f, (float)(_windowSize.second - 200) }).Generate();
		Render();
	}
}
void Window::Refresh() {
	glfwPollEvents();
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
}

void Window::Render() {
	glClear(GL_COLOR_BUFFER_BIT);
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

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

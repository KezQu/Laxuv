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
		glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);

		_window = glfwCreateWindow(_windowSize.first, _windowSize.second, _windowTitle.c_str(), nullptr, nullptr);
		//_window = glfwCreateWindow(_windowSize.first, _windowSize.second, _windowTitle.c_str(), glfwGetPrimaryMonitor(), nullptr);
		if (_window == nullptr) {
			throw std::runtime_error("Unable to create window");
		}
		glfwMakeContextCurrent(_window);
		//glfwSwapInterval(1);

		GLenum errorCode = glewInit();
		if (errorCode != GLEW_OK) {
			std::cerr << "Unable to initialize OpenGL glewInit failed" << std::endl;
			throw std::runtime_error((const char*)(glewGetErrorString(errorCode)));
		}
		Log() << "OpenGL version : " << glGetString(GL_VERSION) << std::endl;
		
		glEnable(GL_DEBUG_OUTPUT);
		glEnable(GL_SCISSOR_TEST);
		glViewport(_windowSize.first / 4.f, 200, 3.f * _windowSize.first / 4.f, _windowSize.second - 220);
		glScissor(_windowSize.first / 4.f, 200, 3.f * _windowSize.first / 4.f, _windowSize.second - 220);

		_(glDebugMessageCallback(Window::OpenGLErrorCallback, nullptr));
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
	VertexArray va(VertexBufferData{ {0, 1, 2}, { -0.5f, -0.5f, 0.0f, 0.5f, 0.5f, 0.0f, 0.5f, -0.5f, 0.0f }, {255, 0, 0, 255, 0, 255, 0, 255, 0, 0, 255, 255} });
	while (glfwWindowShouldClose(_window) == GLFW_FALSE) {
		Refresh();
		//ImGui::ShowDemoWindow();
		Toolbar(ImVec2{ (float)(_windowSize.first),20 }, ImVec2{ 0,0 }).Generate();
		Explorer(ImVec2{ _windowSize.first / 4.f,(float)(_windowSize.first) - 20 }, ImVec2{ 0,20 }).Generate();
		Log(ImVec2{ 3 * _windowSize.first / 4.f, 200 }, ImVec2{ _windowSize.first / 4.f, (float)(_windowSize.second - 200) }).Generate();


		Render();
	}
}
void Window::Refresh() {
	glfwPollEvents();
	FramebufferResizeCheck();
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
}

void Window::Render() {
	static int b = 0;
	b = (b + 1) % 255;
	glClearColor(1.f, 1.f, b / 255.f, 1.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

	glfwSwapBuffers(_window);
}
void Window::FramebufferResizeCheck() {
	glfwGetFramebufferSize(_window, &_windowSize.first, &_windowSize.second);
}

void Window::GLFWErrorCallback(int error, const char* description){
	std::cerr << "Error " << error << ": " << description << std::endl;
}
void Window::OpenGLErrorCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam) {
	Log() << "OpenGL Error :" <<  std::string(message, length) << " - source : " << std::hex << source << std::endl;
}

void Window::GLFWFrameBufferResizeCallback(GLFWwindow* window, int width, int height) {
	if (glfwGetWindowAttrib(window, GLFW_ICONIFIED) == GLFW_FALSE) {
		glViewport(width / 4.f, 200, 3.f * width / 4.f, height - 220);
		glScissor(width / 4.f, 200, 3.f * width / 4.f, height - 220);
	}
}
void Window::GLFWKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, GLFW_TRUE);
	}
}

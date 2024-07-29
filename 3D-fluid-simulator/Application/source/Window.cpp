#include <Window.h>

Window::Window(ImVec2 const& windowSize, std::string const& windowTitle)
	:_windowSize{ windowSize },
	_windowTitle{ windowTitle }
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
		//glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);

		_window = glfwCreateWindow(_windowSize.x, _windowSize.y, _windowTitle.c_str(), nullptr, nullptr);
		//_window = glfwCreateWindow(_windowSize.first, _windowSize.second, _windowTitle.c_str(), glfwGetPrimaryMonitor(), nullptr);
		if (_window == nullptr) {
			throw std::runtime_error("Unable to create window");
		}

		glfwMakeContextCurrent(_window);
		glfwSwapInterval(0);

		GLenum errorCode = glewInit();
		if (errorCode != GLEW_OK) {
			std::cerr << "Unable to initialize OpenGL glewInit failed" << std::endl;
			throw std::runtime_error((const char*)(glewGetErrorString(errorCode)));
		}
		LOG << "OpenGL version : " << glGetString(GL_VERSION) << std::endl;

		glEnable(GL_DEBUG_OUTPUT);
		glEnable(GL_PROGRAM_POINT_SIZE);
		glEnable(GL_SCISSOR_TEST);
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glEnable(GL_CULL_FACE);
		glFrontFace(GL_CW);

		auto viewportSize = CalculateViewport(_windowSize);
		glViewport(viewportSize.x, viewportSize.y, viewportSize.z, viewportSize.w);
		glScissor(viewportSize.x, viewportSize.y, viewportSize.z, viewportSize.w);
		Camera::GetCamera().ProjectionRescale(viewportSize.z, viewportSize.w);

		_(glDebugMessageCallback(Window::OpenGLErrorCallback, nullptr));
		glfwSetFramebufferSizeCallback(_window, Window::GLFWFrameBufferResizeCallback);
		//glfwSetKeyCallback(_window, Window::GLFWKeyCallback);
		//glfwSetInputMode(_window, GLFW_STICKY_KEYS, GLFW_TRUE);

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
	WorldAxes Axes{};
	auto instance = PhysicsDispatch::GetDispatchInstance();
	instance->InitDefaultState(Distribution::LINE, {0,0,0}, 100'000'000U);
	//instance->GenerateFrameForces();

	while (glfwWindowShouldClose(_window) == GLFW_FALSE) {
		Refresh();
		//ImGui::ShowDemoWindow();
		Toolbar(ImVec2{ _windowSize.x, 20 }, ImVec2{ 0,0 }).Generate();
		Explorer(ImVec2{ _windowSize.x / 4.f, _windowSize.y - 20 }, ImVec2{ 0,20 }).Generate();
		Logger(LOG, ImVec2{ 3 * _windowSize.x / 4.f, 200 }, ImVec2{ _windowSize.x / 4.f, _windowSize.y - 200 }).Generate();

		instance->GenerateFrameForces();
		for (auto& [id, entity] : Explorer::EntitiesContainer) {
			entity->Draw();
		}
		Axes.Draw();
		Render();
	}

}
void Window::Refresh() {
	glEnable(GL_PROGRAM_POINT_SIZE);
	glClearColor(0.f, 0.f, 0.f, 1.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	FramebufferResizeCheck();
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
}
void Window::Render() {
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

	glfwSwapBuffers(_window);
	glfwPollEvents();
	ProcessKeyInputs();
}
void Window::FramebufferResizeCheck() {
	int width, height;
	glfwGetFramebufferSize(_window, &width, &height);
	_windowSize = { static_cast<float>(width), static_cast<float>(height) };
}
void Window::ProcessKeyInputs() {
	if (ImGui::IsKeyPressed(ImGuiKey_W)) {
		Camera::GetCamera().Move(ImGuiKey_W);
	}
	if (ImGui::IsKeyPressed(ImGuiKey_S)) {
		Camera::GetCamera().Move(ImGuiKey_S);
	}
	if (ImGui::IsKeyPressed(ImGuiKey_D)) {
		Camera::GetCamera().Move(ImGuiKey_D);
	}
	if (ImGui::IsKeyPressed(ImGuiKey_A)) {
		Camera::GetCamera().Move(ImGuiKey_A);
	}
	if (ImGui::IsKeyPressed(ImGuiKey_E)) {
		Camera::GetCamera().Move(ImGuiKey_E);
	}
	if (ImGui::IsKeyPressed(ImGuiKey_Q)) {
		Camera::GetCamera().Move(ImGuiKey_Q);
	}
	if (ImGui::IsMouseClicked(ImGuiMouseButton_Right)) {
		if (ImGui::IsMouseHoveringRect({ _windowSize.x / 4.f, 20 }, { _windowSize.x, _windowSize.y - 200 }, false)) {
			glfwSetInputMode(glfwGetCurrentContext(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		}
	}
	else if (ImGui::IsMouseDown(ImGuiMouseButton_Right)) {
		if (glfwGetInputMode(glfwGetCurrentContext(), GLFW_CURSOR) == GLFW_CURSOR_DISABLED) {
			static glm::dvec2 mousePosPrev{ 0.0 }, mousePosCurr{ 0.0 }, mouseDeltaPos{ 0.0 };
			glfwGetCursorPos(glfwGetCurrentContext(), &mousePosCurr.x, &mousePosCurr.y);
			mousePosCurr;
			mouseDeltaPos = mousePosCurr - mousePosPrev;

			auto viewportSize = CalculateViewport(_windowSize);
			Camera::GetCamera().Rotate({ mouseDeltaPos.x, mouseDeltaPos.y, 0.f });
			mousePosPrev = mousePosCurr;
		}
	}
	else {
		glfwSetInputMode(glfwGetCurrentContext(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	}
}
void Window::GLFWErrorCallback(int error, const char* description){
	std::cerr << "Error " << error << ": " << description << std::endl;
}
void Window::OpenGLErrorCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam) {
	std::string messageType;
	switch (type) {
	case GL_DEBUG_TYPE_ERROR:
		messageType = "ERROR";
		break;
	case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
		messageType =  "DEPRECATED_BEHAVIOR";
		break;
	case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
		messageType =  "UNDEFINED_BEHAVIOR";
		break;
	case GL_DEBUG_TYPE_PORTABILITY:
		messageType =  "PORTABILITY";
		break;
	case GL_DEBUG_TYPE_PERFORMANCE:
		messageType =  "PERFORMANCE";
		break;
	case GL_DEBUG_TYPE_OTHER:
		messageType =  "OTHER";
		break;
	}
	if(messageType != "OTHER")
		LOG << messageType << ": " << std::string(message, length) << " - source : " << std::hex << source << std::endl;
}
void Window::GLFWFrameBufferResizeCallback(GLFWwindow* window, int width, int height) {
	if (glfwGetWindowAttrib(window, GLFW_ICONIFIED) == GLFW_FALSE) {
		auto viewportSize = CalculateViewport({ static_cast<float>(width), static_cast<float>(height) });
		glViewport(viewportSize.x, viewportSize.y, viewportSize.z, viewportSize.w);
		glScissor(viewportSize.x, viewportSize.y, viewportSize.z, viewportSize.w);
		Camera::GetCamera().ProjectionRescale(viewportSize.z, viewportSize.w);
	}
}
void Window::GLFWKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	if (glfwGetKey(glfwGetCurrentContext(), GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, GLFW_TRUE);
	}
}
ImVec4 Window::CalculateViewport(ImVec2 const& windowSize) {
	return { windowSize.x / 4.f, 200, 3.f * windowSize.x / 4.f, windowSize.y - 220 };
}

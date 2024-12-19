#include "Window.h"

#include <filesystem>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

#include "Camera.h"
#include "Debug.h"
#include "Essentials.h"
#include "Explorer.h"
#include "GL/glew.h"
#include "GLFW/glfw3.h"
#include "Logger.h"
#include "ProgramDispatch.h"
#include "Simulator.h"
#include "Toolbar.h"
#include "WorldAxes.h"
#include "imgui.h"

Window::Window(ImVec2 const& windowSize, std::string const& windowTitle)
    : _windowSize{windowSize}, _windowTitle{windowTitle}
{
  // Initialize GLFW and register callback for easier debugging problems related
  // to window operations
  glfwSetErrorCallback(Window::GLFWErrorCallback);
  if (glfwInit() == GLFW_FALSE)
  {
    std::cerr << "Unable to initialize GLFW context" << std::endl;
    throw std::runtime_error("Unable to initialize GLFW context");
  }
  else
  {
    // Specify OpenGL context version, profiling and enable debuging
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Create main window used for displaying controls and simulation context
    _window = glfwCreateWindow(_windowSize.x, _windowSize.y,
                               _windowTitle.c_str(), nullptr, nullptr);
    if (_window == nullptr)
    {
      throw std::runtime_error("Unable to create window");
    }

    // Set OpenGL context to the current window and enable vertical
    // synchronization for stable frame rate
    glfwMakeContextCurrent(_window);
    glfwSwapInterval(1);

    // Initialize library responsible for providing OpenGL functions
    GLenum errorCode = glewInit();
    if (errorCode != GLEW_OK)
    {
      std::cerr << "Unable to initialize OpenGL glewInit failed" << std::endl;
      throw std::runtime_error((const char*)(glewGetErrorString(errorCode)));
    }
    LOG << "OpenGL version : " << glGetString(GL_VERSION) << std::endl;

    // Enable basic OpenGL features and specify blending function needed for
    // drawing commands
    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_PROGRAM_POINT_SIZE);
    glEnable(GL_SCISSOR_TEST);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_CULL_FACE);
    glFrontFace(GL_CW);

    // Specify the part of a window to be used for rendering simulation frames
    auto viewportSize = CalculateViewport(_windowSize);
    glViewport(viewportSize.x, viewportSize.y, viewportSize.z, viewportSize.w);
    glScissor(viewportSize.x, viewportSize.y, viewportSize.z, viewportSize.w);
    Camera::GetCamera().ProjectionRescale(viewportSize.z, viewportSize.w);

    // Register OpenGL error callback for debugging purposes and window resize
    // callback to recalculate OpenGL context size
    _(glDebugMessageCallback(Window::OpenGLErrorCallback, nullptr));
    glfwSetFramebufferSizeCallback(_window,
                                   Window::GLFWFrameBufferResizeCallback);

    // Initialize Imgui library responsible for user interface and specify
    // configuration options
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

    // Initialize Imgui with OpenGL and GLFW backend for rendering UI
    ImGui_ImplGlfw_InitForOpenGL(_window, true);
    ImGui_ImplOpenGL3_Init();
  }
}

Window::~Window()
{
  // Perform safe destruction of respective libraries to avoid memory leaks
  Simulator::CleanUp();
  ProgramDispatch::CleanUp();
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();
  glfwDestroyWindow(_window);
  glfwTerminate();
}

void Window::EventLoop()
{
  // Initialize X,Y,Z axes and retrieve simulation object responsible for
  // handling drawing and calculating simulation frames
  WorldAxes Axes{};
  auto& SimulationInstance = Simulator::GetInstance();

  // Main event loop running until user requests to close the application window
  while (glfwWindowShouldClose(_window) == GLFW_FALSE)
  {
    // Clear window and generate user interface
    Refresh();
    Toolbar(ImVec2{_windowSize.x, 20}, ImVec2{0, 0}).Generate();
    Explorer(ImVec2{_windowSize.x / 4.f, _windowSize.y - 20}, ImVec2{0, 20})
        .Generate();
    Logger(LOG, ImVec2{3 * _windowSize.x / 4.f, 200},
           ImVec2{_windowSize.x / 4.f, _windowSize.y - 200})
        .Generate();

    // Choose behaviour of the simulator depending on the requested simulation
    // state
    switch (SimulationInstance->GetSimulationState())
    {
      // Skip calculating simulation frames and fall back to drawing entities
      case Essentials::SimulationState::IDLE:
        break;
      // State responsible for initializing or setting entities to initial state
      case Essentials::SimulationState::INIT:
        for (auto& [id, entity] : SimulationInstance->GetEntities())
        {
          entity->Initialize();
        }
        SimulationInstance->SetSimulationState(
            Essentials::SimulationState::IDLE);
        break;
      // State present during simulation runtime, responsible for calculating
      // physics present in the scene
      case Essentials::SimulationState::SIMULATION:
        for (auto& [id, entity] : SimulationInstance->GetEntities())
        {
          entity->Calculate();
        }
        break;
      // State that generates single frame of simulation and falls back to idle
      // state, good for step by step analysis
      case Essentials::SimulationState::GEN_FRAME:
        for (auto& [id, entity] : SimulationInstance->GetEntities())
        {
          entity->Calculate();
        }
        SimulationInstance->SetSimulationState(
            Essentials::SimulationState::IDLE);
        break;
    }
    // Draw all entities present in the scene
    for (auto& [id, entity] : SimulationInstance->GetEntities())
    {
      entity->Draw();
    }
    // Show X,Y,Z axes in the scene for better orientation
    Axes.Draw();
    // Generate drawn frame and user input
    Render();
  }
}

void Window::Refresh()
{
  // Clear OpenGL context from the previous frame, refresh user interface and
  // check whether window size has changed
  glClearColor(0x23 / 255., 0x65 / 255., 0xA0 / 255., 1.f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  FramebufferResizeCheck();
  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplGlfw_NewFrame();
  ImGui::NewFrame();
}

void Window::Render()
{
  // Render created user inteface using OpenGL backend
  ImGui::Render();
  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

  // Display generated frame to the window and process user inputs
  glfwSwapBuffers(_window);
  glfwPollEvents();
  ProcessKeyInputs();
}

void Window::FramebufferResizeCheck()
{
  // Retrieve window size for proper UI generating
  int width, height;
  glfwGetFramebufferSize(_window, &width, &height);
  _windowSize = {static_cast<float>(width), static_cast<float>(height)};
}

void Window::ProcessKeyInputs()
{
  // Process user input responsible camera movement speed
  if (ImGui::IsKeyPressed(ImGuiKey_MouseWheelY) &&
      ImGui::IsMouseDown(ImGuiMouseButton_Right))
  {
    float delta_speed = ImGui::GetKeyData(ImGuiKey_MouseWheelY)->AnalogValue;
    if (ImGui::IsKeyDown(ImGuiKey_LeftShift))
    {
      delta_speed *= 10;
    }
    Camera::GetCamera().AddMoveSpeed(delta_speed);
  }
  // Process input related to camera position
  if (ImGui::IsKeyPressed(ImGuiKey_W))
  {
    Camera::GetCamera().Move(ImGuiKey_W);
  }
  if (ImGui::IsKeyPressed(ImGuiKey_S))
  {
    Camera::GetCamera().Move(ImGuiKey_S);
  }
  if (ImGui::IsKeyPressed(ImGuiKey_D))
  {
    Camera::GetCamera().Move(ImGuiKey_D);
  }
  if (ImGui::IsKeyPressed(ImGuiKey_A))
  {
    Camera::GetCamera().Move(ImGuiKey_A);
  }
  if (ImGui::IsKeyPressed(ImGuiKey_E))
  {
    Camera::GetCamera().Move(ImGuiKey_E);
  }
  if (ImGui::IsKeyPressed(ImGuiKey_Q))
  {
    Camera::GetCamera().Move(ImGuiKey_Q);
  }
  // Process mouse rotation and orientation
  if (ImGui::IsMouseClicked(ImGuiMouseButton_Right))
  {
    if (ImGui::IsMouseHoveringRect({_windowSize.x / 4.f, 20},
                                   {_windowSize.x, _windowSize.y - 200}, false))
    {
      glfwSetInputMode(glfwGetCurrentContext(), GLFW_CURSOR,
                       GLFW_CURSOR_DISABLED);
    }
  }
  else if (ImGui::IsMouseDown(ImGuiMouseButton_Right))
  {
    if (glfwGetInputMode(glfwGetCurrentContext(), GLFW_CURSOR) ==
        GLFW_CURSOR_DISABLED)
    {
      static glm::dvec2 mousePosPrev{0.0}, mousePosCurr{0.0},
          mouseDeltaPos{0.0};
      glfwGetCursorPos(glfwGetCurrentContext(), &mousePosCurr.x,
                       &mousePosCurr.y);
      mouseDeltaPos = (mousePosCurr - mousePosPrev) * 2.;
      Camera::GetCamera().Rotate({mouseDeltaPos.x, mouseDeltaPos.y, 0.f});
      mousePosPrev = mousePosCurr;
    }
  }
  else
  {
    glfwSetInputMode(glfwGetCurrentContext(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);
  }
}

void Window::GLFWErrorCallback(int error, const char* description)
{
  // Log errors related to window context
  std::cerr << "Error " << error << ": " << description << std::endl;
}

void Window::OpenGLErrorCallback(GLenum source, GLenum type, GLuint id,
                                 GLenum severity, GLsizei length,
                                 const GLchar* message, const void* userParam)
{
  std::string messageType;
  // Generate log message based on the error callback message type
  switch (type)
  {
    case GL_DEBUG_TYPE_ERROR:
      messageType = "ERROR";
      break;
    case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
      messageType = "DEPRECATED_BEHAVIOR";
      break;
    case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
      messageType = "UNDEFINED_BEHAVIOR";
      break;
    case GL_DEBUG_TYPE_PORTABILITY:
      messageType = "PORTABILITY";
      break;
    case GL_DEBUG_TYPE_PERFORMANCE:
      messageType = "PERFORMANCE";
      break;
    case GL_DEBUG_TYPE_OTHER:
      messageType = "OTHER";
      break;
  }
  if (messageType != "OTHER")
  {
    LOG << messageType << ": " << std::string(message, length)
        << " - source : " << std::hex << source << std::endl;
    std::cout << messageType << ": " << std::string(message, length)
              << " - source : " << std::hex << source << std::endl;
  }
}
void Window::GLFWFrameBufferResizeCallback(GLFWwindow* window, int width,
                                           int height)
{
  // Update OpenGL rendering context size whether window is not minimized
  if (glfwGetWindowAttrib(window, GLFW_ICONIFIED) == GLFW_FALSE)
  {
    auto viewportSize = CalculateViewport(
        {static_cast<float>(width), static_cast<float>(height)});
    glViewport(viewportSize.x, viewportSize.y, viewportSize.z, viewportSize.w);
    glScissor(viewportSize.x, viewportSize.y, viewportSize.z, viewportSize.w);
    Camera::GetCamera().ProjectionRescale(viewportSize.z, viewportSize.w);
  }
}
void Window::GLFWKeyCallback(GLFWwindow* window, int key, int scancode,
                             int action, int mods)
{
  // Process closing application request
  if (glfwGetKey(glfwGetCurrentContext(), GLFW_KEY_ESCAPE) == GLFW_PRESS)
  {
    glfwSetWindowShouldClose(window, GLFW_TRUE);
  }
}
ImVec4 Window::CalculateViewport(ImVec2 const& windowSize)
{
  // Calculate OpenGL rendering context size based on the window size
  return {windowSize.x / 4.f, 200, 3.f * windowSize.x / 4.f,
          windowSize.y - 220};
}

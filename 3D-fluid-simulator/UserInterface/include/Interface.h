#pragma once

#include <cstdint>

#include "Simulator.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "imgui_internal.h"
#include "imgui_stdlib.h"

/**
 * @brief Abstract class providing an interface to generate user interface to he
 * main window
 *
 */
class Interface
{
 protected:
  ImVec2 _size;
  ImVec2 _position;
  ImGuiWindowFlags _flags;

 public:
  /**
   * @brief Creates an interface instance with a given specification
   *
   * @param size Size of the main imgui window containing user controls
   * @param position relative position of the window specifying top left corner
   * @param flags ImGui flags for used for window configuration
   */
  Interface(ImVec2 const& size, ImVec2 const& position, ImGuiWindowFlags flags);
  virtual ~Interface();
  /**
   * @brief Pure virtual method for generating content to the main window
   *
   */
  virtual void Generate() = 0;
};

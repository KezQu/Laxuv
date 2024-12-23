#pragma once

#include "Interface.h"

/**
 * @brief Class implementing 'Interface' interface, responsible for providing
 * main simulation controls and additional information about the current state
 * of the simulation
 *
 */
class Toolbar : public Interface
{
 private:
  static float static_timestep;

 public:
  /**
   * @brief Construct a new Toolbar object with a specified properties
   *
   * @param size Size of the Toolbar window
   * @param position Relative position of the toolbar inside a main window
   */
  Toolbar(ImVec2 const& size, ImVec2 const& position);
  /**
   * @brief Method used for specifying contents of the toolbar
   *
   */
  void Generate() override;
};

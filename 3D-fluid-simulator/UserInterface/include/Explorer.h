#pragma once

#include <algorithm>
#include <memory>
#include <unordered_map>
#include <vector>

#include "Entity.h"
#include "Interface.h"
#include "Simulator.h"

/**
 * @brief Class implementing 'Interface' interface, responsible for exposing
 * registered entities in the Simulator, Simulator properties as well as
 * selected entity properties
 *
 */
class Explorer : public Interface
{
 public:
  using entity_selector_t =
      std::pair<Essentials::EntityType, Essentials::EntityShape>;
  /**
   * @brief Construct a new Explorer object with a specified properties
   *
   * @param size Size of the Explorer window
   * @param position Relative position of the explorer inside a main window
   */
  Explorer(ImVec2 const& size, ImVec2 const& position);
  /**
   * @brief Method used for specifying contents of the explorer
   *
   */
  void Generate() override;

 private:
  static uint64_t selected;
  static entity_selector_t entity_select;
  /**
   * @brief Helper method responsible for generating explorer part of the window
   *
   */
  void CreateEntityExplorer();
  /**
   * @brief Helper method responsible for generating world and simulation
   * controls
   *
   */
  void CreateWorldControls();
  /**
   * @brief Helper method responsible for generating controls for a selected
   * entity
   *
   */
  void CreateEntityControls();
  /**
   * @brief Helper method responsible for generating interface for creation of
   * the entities
   *
   */
  void CreateEntitySelector();
};
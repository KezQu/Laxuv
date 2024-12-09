#pragma once

#include <algorithm>
#include <memory>
#include <unordered_map>
#include <vector>

#include "Entity.h"
#include "Interface.h"
#include "Simulator.h"


class Explorer : public Interface
{
 public:
  using entity_selector_t =
      std::pair<Essentials::EntityType, Essentials::EntityShape>;

  Explorer(ImVec2 const& size, ImVec2 const& position);
  void Generate() override;

 private:
  static uint64_t selected;
  static entity_selector_t entity_select;

  void CreateEntityExplorer();
  void CreateWorldControls();
  void CreateEntityControls();
  void CreateEntitySelector();
};
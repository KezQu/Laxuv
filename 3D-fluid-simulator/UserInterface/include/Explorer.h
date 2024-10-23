#pragma once

#include <Entity.h>
#include <Interface.h>
#include <Simulator.h>

#include <algorithm>
#include <memory>
#include <unordered_map>
#include <vector>

class Explorer : public Interface
{
 public:
  using entity_selector_t = std::pair<int32_t, int32_t>;

  Explorer(ImVec2 const& size, ImVec2 const& position);
  void Generate() override;

 private:
  static uint64_t selected;
  static entity_selector_t entity_select;

  void CreateEntityExplorer();
  void CreateWorldControls();
  void CreateEntityControls();
  void CreateEntitySelector();
  void SelectEntity();
};
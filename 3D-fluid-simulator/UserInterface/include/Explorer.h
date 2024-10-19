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
  Explorer(ImVec2 const& size, ImVec2 const& position);
  void Generate() override;

 private:
  void CreateProperty(details::details_mapper& info);
};
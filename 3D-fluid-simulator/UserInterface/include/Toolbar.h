#pragma once

#include "Interface.h"

class Toolbar : public Interface
{
 private:
  static float static_timestep;

 public:
  Toolbar(ImVec2 const& size, ImVec2 const& position);
  void Generate() override;
};

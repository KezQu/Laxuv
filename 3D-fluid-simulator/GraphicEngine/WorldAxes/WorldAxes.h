#pragma once

#include "Entity.h"
#include "Line.h"
#include "Square.h"

class WorldAxes : public Entity
{
 private:
  Line _axes[3];

 public:
  WorldAxes();
  void Draw() const;
};

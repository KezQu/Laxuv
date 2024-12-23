#pragma once

#include "Entity.h"
#include "Line.h"
#include "Square.h"

/**
 * @brief Special class implementing Entity to be able to be drawn into the
 * simulation representing X,Y and Z axes for better navigation throughout
 * simulation world
 *
 */
class WorldAxes : public Entity
{
 private:
  Line _axes[3];

 public:
  /**
   * @brief Constructs a default World Axes object
   *
   */
  WorldAxes();
  /**
   * @brief Method responsible for scheduling drawing the world axes
   *
   */
  void Draw() const override;
};

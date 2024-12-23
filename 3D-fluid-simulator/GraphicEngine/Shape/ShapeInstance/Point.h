#pragma once

#include "Essentials.h"
#include "GL/glew.h"
#include "Shape.h"
#include "VertexArray.h"

/**
 * @brief Class providing default point shape
 *
 */
class Point : public Shape<GL_POINTS>
{
 public:
  /**
   * @brief Constructs new point at a given center
   *
   * @param center Center of the point
   * @param radius Radius defining an initial point size
   */
  Point(Vertex center = {{0, 0, 0}}, float radius = 15.f);
  Point(Point const& obj_copy) = delete;
  Point(Point&& obj_move) = default;
  Point& operator=(Point const& obj_copy) = delete;
  Point& operator=(Point&& obj_move) = default;
  ~Point() override = default;
};

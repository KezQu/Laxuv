#pragma once

#include "Entity.h"
#include "Shape.h"

/**
 * @brief Class providing default line shape
 *
 */
class Line : public Shape<GL_LINES>
{
 public:
  /**
   * @brief Construct a new Line object with a given start and end point
   *
   * @param p1 starting point of the line
   * @param p2 end point
   */
  Line(Vertex p1 = {{-1, 0, 0}}, Vertex p2 = {{1, 0, 0}});
  Line(Line const& obj_copy) = delete;
  Line(Line&& obj_move) = default;
  Line& operator=(Line const& obj_copy) = delete;
  Line& operator=(Line&& obj_move) = default;
  ~Line() override = default;
};
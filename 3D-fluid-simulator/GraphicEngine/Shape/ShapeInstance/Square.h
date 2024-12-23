#pragma once

#include "Shape.h"

/**
 * @brief Class providing default square shape
 *
 */
class Square : public Shape<GL_TRIANGLES>
{
 public:
  /**
   * @brief Creates a square at a given center with specified radius
   *
   * @param center Center of the square to be created
   * @param radius Value equivalent to the half of the edge length
   */
  Square(Vertex center = {{0, 0, 0}}, float radius = 15.f);
  Square(Square const& obj_copy) = delete;
  Square(Square&& obj_move) = default;
  Square& operator=(Square const& obj_copy) = delete;
  Square& operator=(Square&& obj_move) = default;
  ~Square() override = default;

 protected:
  /**
   * @brief Helper constructor for easier creation of the VAO out of a given
   * vertices where v0,v1,... indicate vertices of the square starting top-left,
   * going from top to bottom
   *
   * @param v0
   * @param v1
   * @param v2
   * @param v3
   * @param radius
   */
  Square(Vertex v0, Vertex v1, Vertex v2, Vertex v3, float radius);
};
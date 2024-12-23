#pragma once

#include "Shape.h"

/**
 * @brief Class providing default cube shape
 *
 */
class Cube : public Shape<GL_TRIANGLES>
{
 public:
  /**
   * @brief Creates a cube object around a given point
   *
   * @param center Center of the cube
   * @param radius Value equivalent to the half of the length of the edge
   * @param enableTess Flag indicating if the tesselation is needed to create a
   * shape
   */
  Cube(Vertex center = {{0, 0, 0}}, float radius = 15.f,
       bool enableTess = false);
  Cube(Cube const& obj_copy) = delete;
  Cube(Cube&& obj_move) = default;
  Cube& operator=(Cube const& obj_copy) = delete;
  Cube& operator=(Cube&& obj_move) = default;
  ~Cube() override = default;

 protected:
  /**
   * @brief Helper constructor for easier creation of the VAO from the given
   * vertices v1,v2,...
   *
   * @param v0
   * @param v1
   * @param v2
   * @param v3
   * @param v4
   * @param v5
   * @param v6
   * @param v7
   * @param radius
   * @param enableTess
   */
  Cube(Vertex v0, Vertex v1, Vertex v2, Vertex v3, Vertex v4, Vertex v5,
       Vertex v6, Vertex v7, float radius, bool enableTess = false);
};

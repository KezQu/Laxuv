#pragma once

#include <cstdint>
#include <initializer_list>

#include "GL/glew.h"
#include "IndexBuffer.h"
#include "VertexBuffer.h"
#include "glm/ext/vector_float3.hpp"
#include "glm/ext/vector_uint4.hpp"

/**
 * @brief Helper struct that contains all needed buffers to render vertices
 *
 */
struct VertexBufferData
{
  IndexBuffer indexBuffer;
  VertexBuffer<GLfloat, 3> coordinateBuffer;
  VertexBuffer<GLubyte, 4> colorBuffer;
};
/**
 * @brief Helper struct representing vertices in three dimensions
 *
 */
struct Vertex
{
  glm::vec3 coordinate;
  glm::uvec4 color{0x80, 0x80, 0x80, 0xFF};
};
/**
 * @brief Class providing an RAII alternative for OpenGL vertex array objects
 * (VAO)
 *
 */
class VertexArray
{
 public:
  using type = VertexBufferData;

 private:
  type _data;
  uint32_t _id;

 public:
  /**
   * @brief Constructs empty vertex array object
   *
   */
  VertexArray();
  /**
   * @brief Constructs vertex array object with the specified vertex buffer data
   *
   * @param vertexBufferData
   */
  VertexArray(type&& vertexBufferData);
  /**
   * @brief Constructs a vertex array object out of provided vertices with a
   * specified index buffer
   *
   * @param vertices List of vertices to store inside vertex array object
   * @param idxBuffer Index buffer specifying the order of the vertices to be
   * drawn
   */
  VertexArray(std::initializer_list<Vertex> vertices, IndexBuffer&& idxBuffer);
  /**
   * @brief Explicit deletion of a copy constructor to ensure unique VAOs
   *
   * @param objCopy
   */
  VertexArray(VertexArray const& objCopy) = delete;
  VertexArray(VertexArray&& objMove);
  /**
   * @brief Explicit deletion of a copy operator to ensure unique VAOs
   *
   * @param objCopy
   */
  VertexArray& operator=(VertexArray const& objCopy) = delete;
  VertexArray& operator=(VertexArray&& objMove);
  ~VertexArray();
  /**
   * @brief Getter providing read-only access to the underlying VAO data
   *
   * @return VertexBufferData const&
   */
  VertexBufferData const& Data() const;
  /**
   * @brief Binds current vertex array object and contained buffers to the
   * OpenGL context
   */
  void Bind() const;
  /**
   * @brief Unbinds current vertex array object from the OpenGL context
   */
  void Unbind() const;
  /**
   * @brief Returns the number of drawn vertices
   */
  uint64_t Size() const;
  /**
   * @brief Returns internal index buffer type
   *
   * @return GLenum
   */
  GLenum IndexBufferType() const;
  /**
   * @brief Getter for the unique vertex array object ID
   *
   * @return uint32_t const
   */
  uint32_t const ID() const;
};

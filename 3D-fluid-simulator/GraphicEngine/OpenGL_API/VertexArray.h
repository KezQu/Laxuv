#pragma once

#include <IndexBuffer.h>
#include <VertexBuffer.h>

#include <cstdint>
#include <glm/matrix.hpp>

struct VertexBufferData
{
  IndexBuffer indexBuffer;
  VertexBuffer<GLfloat, 3> coordinateBuffer;
  VertexBuffer<GLubyte, 4> colorBuffer;
};
struct Vertex
{
  glm::vec3 coordinate;
  glm::uvec4 color{127, 127, 127, 255};
};
class VertexArray
{
 public:
  using type = VertexBufferData;

 private:
  type _data;
  uint32_t _id;

 public:
  VertexArray();
  VertexArray(type&& vertexBufferData);
  VertexArray(std::initializer_list<Vertex> vertices, IndexBuffer&& idxBuffer);
  VertexArray(VertexArray const& objCopy) = delete;
  VertexArray(VertexArray&& objMove);
  VertexArray& operator=(VertexArray const& objCopy) = delete;
  VertexArray& operator=(VertexArray&& objMove);
  ~VertexArray();
  VertexBufferData const& Data() const;
  void Bind() const;
  void Unbind() const;
  uint64_t Size() const;
  GLenum IndexBufferType() const;
  uint32_t const ID() const;
};

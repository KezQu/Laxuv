#include "VertexArray.h"

VertexArray::VertexArray() : _id{UINT32_MAX} {}

VertexArray::VertexArray(type&& vertexBufferData) : VertexArray()
{
  _data = std::exchange(vertexBufferData, {});
  glCreateVertexArrays(1, &_id);
  _data.coordinateBuffer.DefineAttribute(_id, 0);
  _data.colorBuffer.DefineAttribute(_id, 1);
}
VertexArray::VertexArray(std::initializer_list<Vertex> vertices,
                         IndexBuffer&& idxBuffer)
    : VertexArray()
{
  std::vector<GLfloat> tmpCoordinates;
  std::vector<GLubyte> tmpColors;
  for (auto& vertex : vertices)
  {
    for (int i = 0; i < 3; i++) tmpCoordinates.push_back(vertex.coordinate[i]);
    for (int i = 0; i < 4; i++) tmpColors.push_back(vertex.color[i]);
  }
  _data.indexBuffer = std::move(idxBuffer);
  _data.coordinateBuffer.UpdateBufferMemory(std::move(tmpCoordinates));
  _data.colorBuffer.UpdateBufferMemory(std::move(tmpColors));
  glCreateVertexArrays(1, &_id);
  _data.coordinateBuffer.DefineAttribute(_id, 0);
  _data.colorBuffer.DefineAttribute(_id, 1);
}
VertexArray::VertexArray(VertexArray&& objMove)
    : _data{std::move(objMove._data)},
      _id{std::exchange(objMove._id, UINT32_MAX)}
{
}

VertexArray& VertexArray::operator=(VertexArray&& objMove)
{
  this->~VertexArray();
  _data = std::move(objMove._data);
  _id = std::exchange(objMove._id, UINT32_MAX);
  return *this;
}

VertexArray::~VertexArray()
{
  if (glIsVertexArray(_id))
  {
    glDeleteVertexArrays(1, &_id);
  }
  _id = 0;
}
VertexBufferData const& VertexArray::Data() const
{
  return _data;
}

void VertexArray::Bind() const
{
  if (glIsVertexArray(_id))
  {
    _(glBindVertexArray(_id));
    _data.indexBuffer.Bind();
    _data.colorBuffer.EnableAttribute(_id);
    _data.coordinateBuffer.EnableAttribute(_id);
  }
}

void VertexArray::Unbind() const
{
  _data.coordinateBuffer.DisableAttribute(_id);
  _data.colorBuffer.DisableAttribute(_id);
  _data.indexBuffer.Unbind();
  glBindVertexArray(0);
}

uint64_t VertexArray::Size() const
{
  return _data.indexBuffer.Size();
}
GLenum VertexArray::IndexBufferType() const
{
  return GL_UNSIGNED_INT;
}
uint32_t const VertexArray::ID() const
{
  return _id;
}

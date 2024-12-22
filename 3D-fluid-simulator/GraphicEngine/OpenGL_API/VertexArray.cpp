#include "VertexArray.h"

VertexArray::VertexArray() : _id{UINT32_MAX} {}

VertexArray::VertexArray(type&& vertexBufferData) : VertexArray()
{
  // Save vertex data and request OpenGL to create new VAO and bind it to the
  // _id
  _data = std::exchange(vertexBufferData, {});
  glCreateVertexArrays(1, &_id);
  // Define position and color buffers for the provided vertices
  _data.coordinateBuffer.DefineAttribute(_id, 0);
  _data.colorBuffer.DefineAttribute(_id, 1);
}
VertexArray::VertexArray(std::initializer_list<Vertex> vertices,
                         IndexBuffer&& idxBuffer)
    : VertexArray()
{
  std::vector<GLfloat> tmpCoordinates;
  std::vector<GLubyte> tmpColors;
  // Extract coordinates and colors from the provided vertices and create
  // corresponding buffers
  for (auto& vertex : vertices)
  {
    for (int i = 0; i < 3; i++) tmpCoordinates.push_back(vertex.coordinate[i]);
    for (int i = 0; i < 4; i++) tmpColors.push_back(vertex.color[i]);
  }
  _data.indexBuffer = std::move(idxBuffer);
  _data.coordinateBuffer.UpdateBufferMemory(std::move(tmpCoordinates));
  _data.colorBuffer.UpdateBufferMemory(std::move(tmpColors));
  // Save vertex data and request OpenGL to create new VAO and bind it to the
  // _id
  glCreateVertexArrays(1, &_id);
  // Define position and color buffers for the provided vertices
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
  // Check whether bound VAO to the id is valid and object is not being move to
  // itself
  if ((glIsVertexArray(_id) == GL_TRUE) && (_id != objMove._id))
  {
    this->~VertexArray();
    _id = std::exchange(objMove._id, UINT32_MAX);
    _data = std::move(objMove._data);
  }
  return *this;
}

VertexArray::~VertexArray()
{
  // Delete VAO bound to the id if it is valid
  if (glIsVertexArray(_id) == GL_TRUE)
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
  // Bind current VAO and enable color and position buffers to be accessible for
  // rendering only if the VAO is valid
  if (glIsVertexArray(_id) == GL_TRUE)
  {
    _(glBindVertexArray(_id));
    _data.indexBuffer.Bind();
    _data.colorBuffer.EnableAttribute(_id);
    _data.coordinateBuffer.EnableAttribute(_id);
  }
}

void VertexArray::Unbind() const
{
  // Disable buffers correlated to vertices
  _data.coordinateBuffer.DisableAttribute(_id);
  _data.colorBuffer.DisableAttribute(_id);
  _data.indexBuffer.Unbind();
  // Bind VAO zero which effectively in core OpenGL profile means unbinding
  // previously bound VAO
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

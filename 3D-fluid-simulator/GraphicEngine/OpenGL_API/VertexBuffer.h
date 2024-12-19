#pragma once

#include <type_traits>
#include <utility>

#include "CPUBuffer.h"

template <typename T, GLuint S>
class VertexBuffer : public CPUBuffer<GL_ARRAY_BUFFER, T>
{
 private:
  const uint64_t size = S;
  int32_t _bindingIndex{-1};

 public:
  using type = T;
  VertexBuffer();
  VertexBuffer(std::initializer_list<type> data);
  VertexBuffer(VertexBuffer const& objCopy) = delete;
  VertexBuffer(VertexBuffer&& objMove);
  VertexBuffer& operator=(VertexBuffer const& objCopy) = delete;
  VertexBuffer& operator=(VertexBuffer&& objMove);
  ~VertexBuffer() override = default;
  void DefineAttribute(GLuint const vertexArrayID, GLuint const bindingIndex);
  void EnableAttribute(GLuint const vertexArrayID) const;
  void DisableAttribute(GLuint const vertexArrayID) const;
  GLuint AttribSize() const;

 private:
  template <
      typename TypeTest = type,
      std::enable_if_t<std::is_same<GLfloat, TypeTest>::value, bool> = true>
  GLenum CheckType() const
  {
    return GL_FLOAT;
  }
  template <
      typename TypeTest = type,
      std::enable_if_t<std::is_same<GLuint, TypeTest>::value, bool> = true>
  GLenum CheckType() const
  {
    return GL_UNSIGNED_INT;
  }
  template <
      typename TypeTest = type,
      std::enable_if_t<std::is_same<GLubyte, TypeTest>::value, bool> = true>
  GLenum CheckType() const
  {
    return GL_UNSIGNED_BYTE;
  }
};

template <typename T, GLuint S>
inline VertexBuffer<T, S>::VertexBuffer() : CPUBuffer<GL_ARRAY_BUFFER, type>()
{
  glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &_bindingIndex);
}

template <typename T, GLuint S>
inline VertexBuffer<T, S>::VertexBuffer(std::initializer_list<type> data)
    : CPUBuffer<GL_ARRAY_BUFFER, type>(data)
{
  if (data.size() % size != 0)
  {
    LOG << "Error size of vertex data is not a multiplication of vertex size("
        << size << ")" << std::endl;
  }
  glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &_bindingIndex);
}

template <typename T, GLuint S>
inline VertexBuffer<T, S>::VertexBuffer(VertexBuffer<T, S>&& objMove)
    : CPUBuffer<GL_ARRAY_BUFFER, type>(std::move(objMove)),
      _bindingIndex{std::exchange(objMove._bindingIndex, {})}
{
}

template <typename T, GLuint S>
inline VertexBuffer<T, S>& VertexBuffer<T, S>::operator=(
    VertexBuffer<T, S>&& objMove)
{
  this->~VertexBuffer();
  this->_id = std::exchange(objMove._id, 0);
  this->_data = std::exchange(objMove._data, {});
  this->_bindingIndex = std::exchange(objMove._bindingIndex, 0);
  return *this;
}

template <typename T, GLuint S>
void inline VertexBuffer<T, S>::DefineAttribute(GLuint const vertexArrayID,
                                                GLuint const bindingIndex)
{
  _bindingIndex = bindingIndex;
  glVertexArrayVertexBuffer(vertexArrayID, _bindingIndex, this->_id, 0,
                            size * sizeof(type));
  glVertexArrayAttribFormat(vertexArrayID, _bindingIndex, size, CheckType(),
                            GL_FALSE, 0);
  glVertexArrayAttribBinding(vertexArrayID, _bindingIndex, bindingIndex);
}

template <typename T, GLuint S>
void inline VertexBuffer<T, S>::EnableAttribute(
    GLuint const vertexArrayID) const
{
  _(glEnableVertexArrayAttrib(vertexArrayID, _bindingIndex));
}

template <typename T, GLuint S>
void inline VertexBuffer<T, S>::DisableAttribute(
    GLuint const vertexArrayID) const
{
  glDisableVertexArrayAttrib(vertexArrayID, _bindingIndex);
}

template <typename T, GLuint S>
GLuint VertexBuffer<T, S>::AttribSize() const
{
  return size;
}

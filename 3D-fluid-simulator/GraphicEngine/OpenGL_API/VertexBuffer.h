#pragma once

#include <concepts>
#include <utility>

#include "CPUBuffer.h"

/**
 * @brief Class representing RAII wrapper for the OpenGL vertex buffer object
 * (VBO), responsible for data used for vertices, providing an input for vertex
 * shaders
 *
 * @tparam T Type of the data stored in the buffer
 * @tparam S Number of the elements ofType T assigned to a single vertex
 */
template <typename Type, GLuint S>
class VertexBuffer : public CPUBuffer<GL_ARRAY_BUFFER, Type>
{
 private:
  const uint64_t size = S;
  int32_t _bindingIndex{-1};

 public:
  /**
   * @brief Constructs empty vertex buffer object
   *
   */
  VertexBuffer();
  /**
   * @brief Constructs VBO with the specified data
   *
   * @param data data to be stored within buffer memory
   */
  VertexBuffer(std::initializer_list<Type> data);
  /**
   * @brief Explicit deletion of the copy constructor to prevent copying the
   * buffer without initializing new OpenGL buffer instance
   *
   * @param objCopy
   */
  VertexBuffer(VertexBuffer const& objCopy) = delete;
  VertexBuffer(VertexBuffer&& objMove);
  /**
   * @brief Explicit deletion of the copy operator to prevent copying the
   * buffer without initializing new OpenGL buffer instance
   *
   * @param objCopy
   */
  VertexBuffer& operator=(VertexBuffer const& objCopy) = delete;
  VertexBuffer& operator=(VertexBuffer&& objMove);
  ~VertexBuffer() override = default;
  /**
   * @brief Method responsible for defining the memory alignment of the buffer
   * and binding point inside the VAO
   *
   * @param vertexArrayID Id of the vertex array object bound to it
   * @param bindingIndex Index of the binding point inside the VAO
   */
  void DefineAttribute(GLuint const vertexArrayID, GLuint const bindingIndex);
  /**
   * @brief Helper method used to enable specific attribute for the given vertex
   * array object
   *
   * @param vertexArrayID Id of the vertex array to be used
   */
  void EnableAttribute(GLuint const vertexArrayID) const;
  /**
   * @brief Helper method used to disable specific attribute for the given
   * vertex array object
   *
   * @param vertexArrayID Id of the vertex array to be used
   */
  void DisableAttribute(GLuint const vertexArrayID) const;
  /**
   * @brief Getter for an attribute size for a single vertex
   *
   * @return GLuint
   */
  GLuint AttribSize() const;

 private:
  /**
   * @brief SFINAE helper method used to retrieve underlying vertex buffer data
   *Type
   *
   * @return GLenum
   */
  GLenum CheckType() const
    requires std::same_as<float, Type>
  {
    return GL_FLOAT;
  }
  GLenum CheckType() const
    requires std::same_as<uint32_t, Type>
  {
    return GL_UNSIGNED_INT;
  }
  GLenum CheckType() const
    requires std::same_as<uint8_t, Type>
  {
    return GL_UNSIGNED_BYTE;
  }
};

template <typename Type, GLuint S>
inline VertexBuffer<Type, S>::VertexBuffer()
    : CPUBuffer<GL_ARRAY_BUFFER, Type>()
{
  // Get maximum binding index value to ensure that default corresponds to
  // invalid binding point
  glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &_bindingIndex);
}

template <typename Type, GLuint S>
inline VertexBuffer<Type, S>::VertexBuffer(std::initializer_list<Type> data)
    : CPUBuffer<GL_ARRAY_BUFFER, Type>(data)
{
  // Check if the provided data contains data for valid number of vertices if no
  // log the message to application console
  if (data.size() % size != 0)
  {
    LOG << "Error size of vertex data is not a multiplication of vertex size("
        << size << ")" << std::endl;
  }
  glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &_bindingIndex);
}

template <typename Type, GLuint S>
inline VertexBuffer<Type, S>::VertexBuffer(VertexBuffer<Type, S>&& objMove)
    : CPUBuffer<GL_ARRAY_BUFFER, Type>(std::move(objMove)),
      _bindingIndex{std::exchange(objMove._bindingIndex, {})}
{
}

template <typename Type, GLuint S>
inline VertexBuffer<Type, S>& VertexBuffer<Type, S>::operator=(
    VertexBuffer<Type, S>&& objMove)
{
  // Check if provided VBO id points to valid buffer object and prevent moving
  // ifself
  if ((glIsBuffer(this->_id) == GL_TRUE) && (this->_id != objMove._id))
  {
    this->~VertexBuffer();
    this->_id = std::exchange(objMove._id, 0);
    this->_data = std::exchange(objMove._data, {});
    this->_bindingIndex = std::exchange(objMove._bindingIndex, 0);
  }
  return *this;
}

template <typename Type, GLuint S>
void inline VertexBuffer<Type, S>::DefineAttribute(GLuint const vertexArrayID,
                                                   GLuint const bindingIndex)
{
  _bindingIndex = bindingIndex;
  // Bind current vertex buffer object to the specified vertex array object,
  // define offset and size in bytes for a single element within the buffer
  glVertexArrayVertexBuffer(vertexArrayID, _bindingIndex, this->_id, 0,
                            size * sizeof(Type));
  // Specify data format at a given binding index, dataType, normalization and
  // relative offset to data within the element
  glVertexArrayAttribFormat(vertexArrayID, _bindingIndex, size, CheckType(),
                            GL_FALSE, 0);
  // Map the attribute index to the binding index for a given vertex array
  // object
  glVertexArrayAttribBinding(vertexArrayID, _bindingIndex, bindingIndex);
}

template <typename Type, GLuint S>
void inline VertexBuffer<Type, S>::EnableAttribute(
    GLuint const vertexArrayID) const
{
  // Enables specific attribute for the given vertex array object
  _(glEnableVertexArrayAttrib(vertexArrayID, _bindingIndex));
}

template <typename Type, GLuint S>
void inline VertexBuffer<Type, S>::DisableAttribute(
    GLuint const vertexArrayID) const
{
  // Disables specific attribute for the given vertex array object
  glDisableVertexArrayAttrib(vertexArrayID, _bindingIndex);
}

template <typename Type, GLuint S>
GLuint VertexBuffer<Type, S>::AttribSize() const
{
  return size;
}

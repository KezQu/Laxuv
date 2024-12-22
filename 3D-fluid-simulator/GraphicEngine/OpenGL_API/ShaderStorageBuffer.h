#pragma once

#include <string>

#include "GPUBuffer.h"

/**
 * @brief Class providing RAII functionality for the OpenGL shader storage
 * buffer
 *
 * @tparam T Type of the data stored in the buffer
 * @tparam BufferT buffer type used as a base class, defaults to GPUBuffer
 */
template <typename T, template <GLenum, typename> typename BufferT = GPUBuffer>
class ShaderStorageBuffer : public BufferT<GL_SHADER_STORAGE_BUFFER, T>
{
 private:
  std::string _buffer_name;

 public:
  using BufferType = T;
  /**
   * @brief Creates new empty buffer with a given name
   *
   * @param buffer_name name of the buffer to indicate the binding point in
   * shaders
   */
  explicit ShaderStorageBuffer(std::string buffer_name);
  /**
   * @brief Creates new buffer with a given name and size
   *
   * @param buffer_name name of the buffer to indicate the binding point in
   * shaders
   * @param initialBufferSize initial size of the buffer
   */
  explicit ShaderStorageBuffer(std::string buffer_name,
                               uint64_t initialBufferSize);
  /**
   * @brief Explicit deletion of copy constructor to disallow copying buffers
   *
   * @param objCopy
   */
  ShaderStorageBuffer(ShaderStorageBuffer const& objCopy) = delete;
  explicit ShaderStorageBuffer(ShaderStorageBuffer&& objMove) = default;
  /**
   * @brief Explicit deletion of copy operator to disallow copying buffers
   *
   * @param objCopy
   */
  ShaderStorageBuffer& operator=(ShaderStorageBuffer const& objCopy) = delete;
  ShaderStorageBuffer& operator=(ShaderStorageBuffer&& objMove) = default;
  ~ShaderStorageBuffer() override = default;

  /**
   * @brief Retrieve data from the buffer
   *
   * @param offset offset to the buffer
   * @param payloadSize number of elements to read offset + payloadSize
   * @return std::vector<T>
   */
  std::vector<T> GetBufferSubData(uint64_t offset, uint64_t payloadSize) const;
  /**
   * @brief Bind buffer to the specified program
   *
   * @param programID program id to bind the buffer to
   */
  void Bind(uint64_t const& programID) const;
  /**
   * @brief Unbinds the buffer from the specified program
   *
   * @param programID program id to unbind from
   */
  void Unbind(uint64_t const& programID) const;
};

template <typename T, template <GLenum, typename> typename BufferT>
inline ShaderStorageBuffer<T, BufferT>::ShaderStorageBuffer(
    std::string buffer_name)
    : _buffer_name{buffer_name}
{
}

template <typename T, template <GLenum, typename> typename BufferT>
inline ShaderStorageBuffer<T, BufferT>::ShaderStorageBuffer(
    std::string buffer_name, uint64_t initialBufferSize)
    : BufferT<GL_SHADER_STORAGE_BUFFER, T>{initialBufferSize},
      _buffer_name{buffer_name}
{
}

template <typename T, template <GLenum, typename> typename BufferT>
inline std::vector<T> ShaderStorageBuffer<T, BufferT>::GetBufferSubData(
    uint64_t objectsOffset, uint64_t numberOfObjectsToRead) const
{
  std::vector<T> bufferSubData(numberOfObjectsToRead);
  // Retrieve data from the buffer if the buffer bound to the id is valid
  if (glIsBuffer(this->ID()) == GL_TRUE)
  {
    _(glGetNamedBufferSubData(this->ID(), objectsOffset * sizeof(T),
                              numberOfObjectsToRead * sizeof(T),
                              bufferSubData.data()));
  }
  return bufferSubData;
}

template <typename T, template <GLenum, typename> typename BufferT>
inline void ShaderStorageBuffer<T, BufferT>::Bind(
    uint64_t const& programID) const
{
  // Bind buffer
  BufferT<GL_SHADER_STORAGE_BUFFER, T>::Bind();
  // Retrieve binding index from a pipeline based on a shader buffer name
  auto bindingIndex = _(glGetProgramResourceIndex(
      programID, GL_SHADER_STORAGE_BLOCK, _buffer_name.c_str()));
  // Bind buffer to the binding index if retrieved value was valid
  if (bindingIndex != GL_INVALID_INDEX)
  {
    _(glBindBufferBase(GL_SHADER_STORAGE_BUFFER, bindingIndex, this->_id));
  }
}

template <typename T, template <GLenum, typename> typename BufferT>
inline void ShaderStorageBuffer<T, BufferT>::Unbind(
    uint64_t const& programID) const
{
  // Unbind buffer
  BufferT<GL_SHADER_STORAGE_BUFFER, T>::Unbind();
  // Bind buffer under id 0 to the binding index effectively unbinds previously
  // bound buffer
  auto bindingIndex = _(glGetProgramResourceIndex(
      programID, GL_SHADER_STORAGE_BLOCK, _buffer_name.c_str()));
  _(glBindBufferBase(GL_SHADER_STORAGE_BUFFER, bindingIndex, 0));
}

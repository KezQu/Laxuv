#pragma once

#include <string>

#include "GPUBuffer.h"

template <typename T, template <GLenum, typename> typename BufferT = GPUBuffer>
class ShaderStorageBuffer : public BufferT<GL_SHADER_STORAGE_BUFFER, T>
{
 private:
  std::string _buffer_name;

 public:
  using BufferType = T;
  explicit ShaderStorageBuffer(std::string buffer_name);
  explicit ShaderStorageBuffer(std::string buffer_name,
                               uint64_t initialBufferSize);
  ShaderStorageBuffer(ShaderStorageBuffer const& objCopy) = delete;
  explicit ShaderStorageBuffer(ShaderStorageBuffer&& objMove) = default;
  ShaderStorageBuffer& operator=(ShaderStorageBuffer const& objCopy) = delete;
  ShaderStorageBuffer& operator=(ShaderStorageBuffer&& objMove) = default;
  ~ShaderStorageBuffer() override = default;

  std::vector<T> GetBufferSubData(uint64_t offset, uint64_t payloadSize) const;
  void Bind(uint64_t const& programID) const;
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
  BufferT<GL_SHADER_STORAGE_BUFFER, T>::Bind();
  auto bindingIndex = _(glGetProgramResourceIndex(
      programID, GL_SHADER_STORAGE_BLOCK, _buffer_name.c_str()));
  if (bindingIndex != GL_INVALID_INDEX)
  {
    _(glBindBufferBase(GL_SHADER_STORAGE_BUFFER, bindingIndex, this->_id));
  }
}

template <typename T, template <GLenum, typename> typename BufferT>
inline void ShaderStorageBuffer<T, BufferT>::Unbind(
    uint64_t const& programID) const
{
  BufferT<GL_SHADER_STORAGE_BUFFER, T>::Unbind();
  auto bindingIndex = _(glGetProgramResourceIndex(
      programID, GL_SHADER_STORAGE_BLOCK, _buffer_name.c_str()));
  _(glBindBufferBase(GL_SHADER_STORAGE_BUFFER, bindingIndex, 0));
}

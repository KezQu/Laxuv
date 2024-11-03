#pragma once

#include "BufferI.h"

template <GLenum target, typename T>
class GPUBuffer : public BufferI<target, T>
{
 protected:
  uint64_t _bufferSize;

 public:
  const GLenum Target{target};

 protected:
  explicit GPUBuffer();
  explicit GPUBuffer(uint64_t initialBufferSize);
  GPUBuffer(GPUBuffer const& objCopy) = delete;
  explicit GPUBuffer(GPUBuffer&& objMove) = default;
  GPUBuffer& operator=(GPUBuffer const& objCopy) = delete;
  GPUBuffer& operator=(GPUBuffer&& objMove) = default;
  ~GPUBuffer() override = default;

 public:
  T const* const GetBufferMemory() const override;
  T* const GetBufferMemory() override;
  uint64_t Size() const final;
  void SetBufferMemorySize(uint64_t newBufferMemorySize);
};

template <GLenum target, typename T>
inline GPUBuffer<target, T>::GPUBuffer() : BufferI<target, T>{GL_STATIC_DRAW}
{
}

template <GLenum target, typename T>
inline GPUBuffer<target, T>::GPUBuffer(uint64_t initialBufferSize)
    : BufferI<target, T>{GL_STATIC_DRAW}, _bufferSize{initialBufferSize}
{
  _(glNamedBufferData(this->_id, Size() * sizeof(T), GetBufferMemory(),
                      this->_purpose));
}

template <GLenum target, typename T>
inline T const* const GPUBuffer<target, T>::GetBufferMemory() const
{
  return nullptr;
}

template <GLenum target, typename T>
inline T* const GPUBuffer<target, T>::GetBufferMemory()
{
  return nullptr;
}

template <GLenum target, typename T>
inline uint64_t GPUBuffer<target, T>::Size() const
{
  return _bufferSize;
}

template <GLenum target, typename T>
inline void GPUBuffer<target, T>::SetBufferMemorySize(
    uint64_t newBufferMemorySize)
{
  _bufferSize = newBufferMemorySize;
  _(glNamedBufferData(this->_id, Size() * sizeof(T), GetBufferMemory(),
                      this->_purpose));
}

#pragma once

#include "BufferI.h"

/**
 * @brief Class implementing abstract class BufferI which stores data directly
 * in the GPU memory
 *
 * @tparam target Type of a buffer
 * @tparam T Type of data to be stored inside buffers' memory
 */
template <GLenum target, typename T>
class GPUBuffer : public BufferI<target, T>
{
 protected:
  uint64_t _bufferSize;

 public:
  const GLenum Target{target};

 protected:
  /**
   * @brief Create a new buffer without any data
   *
   */
  explicit GPUBuffer();
  /**
   * @brief Create new buffer with initial buffer size
   *
   * @param initialBufferSize initial size of the buffer to be created on the
   * GPU
   */
  explicit GPUBuffer(uint64_t initialBufferSize);
  GPUBuffer(GPUBuffer const& objCopy) = delete;
  explicit GPUBuffer(GPUBuffer&& objMove) = default;
  GPUBuffer& operator=(GPUBuffer const& objCopy) = delete;
  GPUBuffer& operator=(GPUBuffer&& objMove) = default;
  ~GPUBuffer() override = default;

 public:
  T const* const GetBufferMemory() const override;
  T* const GetBufferMemory() override;
  /**
   * @brief Retrieve size of the buffer in bytes
   *
   * @return uint64_t
   */
  uint64_t Size() const final;
  /**
   * @brief Set the Buffer Memory Size object
   *
   * @param newBufferMemorySize new buffer size
   */
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
  // Initializes buffer for a given purpose with a specified size
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
  // Store new size in the memory and reinitialize buffer with new size
  _bufferSize = newBufferMemorySize;
  _(glNamedBufferData(this->_id, Size() * sizeof(T), GetBufferMemory(),
                      this->_purpose));
}

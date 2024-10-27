#pragma once

#include <BufferI.h>

#include <utility>

template <GLenum target, typename T = void>
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
inline GPUBuffer<target, T>::GPUBuffer() : BufferI<target, T>{}
{
}

template <GLenum target, typename T>
inline GPUBuffer<target, T>::GPUBuffer(uint64_t initialBufferSize)
    : BufferI<target, T>{GL_STATIC_DRAW}, _bufferSize{initialBufferSize}
{
  _(glNamedBufferData(this->_id, Size() * sizeof(T), GetBufferMemory(),
                      this->_purpose));
}

// template <GLenum target, typename T>
// inline GPUBuffer<target, T>::GPUBuffer(GPUBuffer<target, T>&& objMove)
//     : BufferI<target, T>{std::move(objMove)},
//       _bufferSize{std::exchange(objMove._bufferSize, {})} {}

// template <GLenum target, typename T>
// inline GPUBuffer<target, T>& GPUBuffer<target, T>::operator=(
//     GPUBuffer<target, T>&& objMove) {
//   this->~GPUBuffer();
//   this->_id = std::exchange(objMove._id, UINT32_MAX);
//   this->_purpose = std::exchange(objMove._purpose, 0U);
//   this->_bufferSize = std::exchange(objMove._bufferSize, {});
//   return *this;
// }

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
  if (!glIsBuffer(this->_id))
  {
    this->CreateBuffer(GL_STATIC_DRAW);
  }
  _bufferSize = newBufferMemorySize;
  _(glNamedBufferData(this->_id, Size() * sizeof(T), GetBufferMemory(),
                      this->_purpose));
}

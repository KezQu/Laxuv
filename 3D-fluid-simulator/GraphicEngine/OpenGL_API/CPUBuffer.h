#pragma once

#include <cstdint>
#include <initializer_list>
#include <vector>

#include "BufferI.h"
#include "Debug.h"
#include "GL/glew.h"

template <GLenum target, typename T>
class CPUBuffer : public BufferI<target, T>
{
 protected:
  std::vector<T> _data;

 protected:
  explicit CPUBuffer();
  CPUBuffer(std::initializer_list<T> data);
  CPUBuffer(std::vector<T>&& data);
  CPUBuffer(CPUBuffer const& objCopy) = delete;
  explicit CPUBuffer(CPUBuffer&& objMove) = default;
  CPUBuffer& operator=(CPUBuffer const& objCopy) = delete;
  CPUBuffer& operator=(CPUBuffer&& objMove) = default;
  ~CPUBuffer() override = default;

 public:
  std::vector<T> const& Data() const;
  std::vector<T>& Data();
  T const* const GetBufferMemory() const override;
  T* const GetBufferMemory() override;
  uint64_t Size() const final;
  void UpdateBufferMemory(std::vector<T>&& data);
  void UpdateBufferMemory(std::initializer_list<T> data);
  void AddToBufferMemory(T const& new_element);
  void RemoveFromBufferMemory(uint32_t const element_id);
};

template <GLenum target, typename T>
inline CPUBuffer<target, T>::CPUBuffer() : BufferI<target, T>{GL_STATIC_DRAW}
{
}

template <GLenum target, typename T>
inline CPUBuffer<target, T>::CPUBuffer(std::initializer_list<T> data)
    : BufferI<target, T>{GL_STATIC_DRAW}, _data{data}
{
  _(glNamedBufferData(this->_id, Size() * sizeof(T), GetBufferMemory(),
                      this->_purpose));
}

template <GLenum target, typename T>
inline CPUBuffer<target, T>::CPUBuffer(std::vector<T>&& data)
    : BufferI<target, T>{GL_STATIC_DRAW}, _data{data}
{
  _(glNamedBufferData(this->_id, Size() * sizeof(T), GetBufferMemory(),
                      this->_purpose));
}

template <GLenum target, typename T>
inline std::vector<T> const& CPUBuffer<target, T>::Data() const
{
  return _data;
}

template <GLenum target, typename T>
inline std::vector<T>& CPUBuffer<target, T>::Data()
{
  return _data;
}

template <GLenum target, typename T>
inline T const* const CPUBuffer<target, T>::GetBufferMemory() const
{
  return _data.data();
}

template <GLenum target, typename T>
inline T* const CPUBuffer<target, T>::GetBufferMemory()
{
  return _data.data();
}

template <GLenum target, typename T>
inline void CPUBuffer<target, T>::UpdateBufferMemory(std::vector<T>&& data)
{
  _data = std::move(data);
  _(glNamedBufferData(this->_id, Size() * sizeof(T), GetBufferMemory(),
                      this->_purpose));
}

template <GLenum target, typename T>
inline void CPUBuffer<target, T>::UpdateBufferMemory(
    std::initializer_list<T> data)
{
  UpdateBufferMemory(std::vector<T>(std::begin(data), std::end(data)));
}

template <GLenum target, typename T>
inline void CPUBuffer<target, T>::AddToBufferMemory(T const& new_element)
{
  _data.push_back(new_element);
  _(glNamedBufferData(this->_id, Size() * sizeof(T), GetBufferMemory(),
                      this->_purpose));
}

template <GLenum target, typename T>
inline void CPUBuffer<target, T>::RemoveFromBufferMemory(
    uint32_t const element_id)
{
  _data.erase(_data.begin() + element_id);
  _(glNamedBufferData(this->_id, Size() * sizeof(T), GetBufferMemory(),
                      this->_purpose));
}

template <GLenum target, typename T>
inline uint64_t CPUBuffer<target, T>::Size() const
{
  return _data.size();
}

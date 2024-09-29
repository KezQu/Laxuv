#pragma once

#include <BufferI.h>

#include <utility>

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
};

template <GLenum target, typename T>
inline CPUBuffer<target, T>::CPUBuffer() : BufferI<target, T>{}
{
}

template <GLenum target, typename T>
inline CPUBuffer<target, T>::CPUBuffer(std::initializer_list<T> data)
    : BufferI<target, T>(GL_STATIC_DRAW), _data(data)
{
  _(glNamedBufferData(this->_id, Size() * sizeof(T), GetBufferMemory(),
                      this->_purpose));
}

template <GLenum target, typename T>
inline CPUBuffer<target, T>::CPUBuffer(std::vector<T>&& data)
    : BufferI<target, T>(GL_STATIC_DRAW), _data(data)
{
  _(glNamedBufferData(this->_id, Size() * sizeof(T), GetBufferMemory(),
                      this->_purpose));
}

// template <GLenum target, typename T>
// inline CPUBuffer<target, T>::CPUBuffer(CPUBuffer<target, T>&& objMove)
//     : BufferI<target, T>{std::move(objMove)},
//       _data{std::exchange(objMove._data, {})} {}

// template <GLenum target, typename T>
// inline CPUBuffer<target, T>& CPUBuffer<target, T>::operator=(
//     CPUBuffer<target, T>&& objMove) {
//   this->~CPUBuffer();
//   this->_id = std::exchange(objMove._id, UINT32_MAX);
//   this->_purpose = std::exchange(objMove._purpose, 0U);
//   this->_data = std::exchange(objMove._data, {});
//   return *this;
// }

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
  if (!glIsBuffer(this->_id))
  {
    this->CreateBuffer(GL_STATIC_DRAW);
  }
  _data = std::move(data);
  glNamedBufferData(this->_id, _data.size() * sizeof(T), GetBufferMemory(),
                    this->_purpose);
}

template <GLenum target, typename T>
inline void CPUBuffer<target, T>::UpdateBufferMemory(
    std::initializer_list<T> data)
{
  UpdateBufferMemory(std::vector<T>(std::begin(data), std::end(data)));
}

template <GLenum target, typename T>
inline uint64_t CPUBuffer<target, T>::Size() const
{
  return _data.size();
}

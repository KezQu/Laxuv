#pragma once

#include <utility>
#include <vector>

#include "Debug.h"
#include "GL/glew.h"

template <GLenum target, typename T>
class BufferI
{
 protected:
  uint32_t _id{UINT32_MAX};
  GLenum _purpose{GL_STATIC_DRAW};

 public:
  const GLenum Target{target};

 protected:
  void CreateBuffer(GLenum purpose);
  explicit BufferI(GLenum purpose);
  BufferI(BufferI const& objCopy) = delete;
  explicit BufferI(BufferI&& objMove);
  BufferI& operator=(BufferI const& objCopy) = delete;
  BufferI& operator=(BufferI&& objMove);
  virtual ~BufferI();

 public:
  void Bind() const;
  void Unbind() const;
  uint64_t const ID() const;
  virtual T const* const GetBufferMemory() const = 0;
  virtual T* const GetBufferMemory() = 0;
  virtual uint64_t Size() const = 0;
};

template <GLenum target, typename T>
inline void BufferI<target, T>::CreateBuffer(GLenum purpose)
{
  if (glIsBuffer(_id) == GL_FALSE)
  {
    _(glCreateBuffers(1, &_id));
  }
  _purpose = purpose;
}

template <GLenum target, typename T>
inline BufferI<target, T>::BufferI(GLenum purpose) : _purpose{purpose}
{
  CreateBuffer(purpose);
}

template <GLenum target, typename T>
inline BufferI<target, T>::BufferI(BufferI<target, T>&& objMove)
    : _id{std::exchange(objMove._id, UINT32_MAX)},
      _purpose{std::exchange(objMove._purpose, 0U)}
{
  CreateBuffer(this->_purpose);
}

template <GLenum target, typename T>
inline BufferI<target, T>& BufferI<target, T>::operator=(
    BufferI<target, T>&& objMove)
{
  if (this->_id != UINT32_MAX)
  {
    this->~BufferI();
  }
  this->_id = std::exchange(objMove._id, UINT32_MAX);
  this->_purpose = std::move(objMove._purpose);
  return *this;
}

template <GLenum target, typename T>
inline BufferI<target, T>::~BufferI()
{
  if (glIsBuffer(_id))
  {
    _(glDeleteBuffers(1, &_id));
  }
  _id = UINT32_MAX;
  _purpose = 0;
}

template <GLenum target, typename T>
inline void BufferI<target, T>::Bind() const
{
  if (glIsBuffer(_id))
  {
    _(glBindBuffer(target, _id));
  }
}

template <GLenum target, typename T>
inline void BufferI<target, T>::Unbind() const
{
  _(glBindBuffer(target, 0));
}
template <GLenum target, typename T>
inline uint64_t const BufferI<target, T>::ID() const
{
  return _id;
}

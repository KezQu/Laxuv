#pragma once

#include <utility>
#include <vector>

#include "Debug.h"
#include "GL/glew.h"

/**
 * @brief Class representing an abstract buffer that provides RAII API for
 * OpenGL buffers
 *
 * @tparam target OpenGL buffer type
 * @tparam T Type representing data present in the buffer
 */
template <GLenum target, typename T>
class BufferI
{
 protected:
  uint32_t _id{UINT32_MAX};
  GLenum _purpose{GL_STATIC_DRAW};

 public:
  const GLenum Target{target};

 protected:
  /**
   * @brief Helper method to create an OpenGL buffer for a given purpose
   *
   * @param purpose purpose of the buffer which is a hint to OpenGL how to
   * optimize future calls related to the buffer
   */
  void CreateBuffer(GLenum purpose);
  /**
   * @brief Constructs an instance of a buffer for a given purpose
   *
   * @param purpose purpose of the buffer which is a hint to OpenGL how to
   * optimize future calls related to the buffer
   */
  explicit BufferI(GLenum purpose);
  /**
   * @brief Explicit deletion of copy constructor to ensure that
   * every buffer bound to the specific id is unique
   *
   * @param objCopy
   */
  BufferI(BufferI const& objCopy) = delete;
  explicit BufferI(BufferI&& objMove);
  /**
   * @brief Explicit deletion of copy operator to ensure that
   * every buffer bound to the specific id is unique
   *
   * @param objCopy
   */
  BufferI& operator=(BufferI const& objCopy) = delete;
  BufferI& operator=(BufferI&& objMove);
  /**
   * @brief Destructor to provide RAII deletion of a bound buffer to the id
   *
   */
  virtual ~BufferI();

 public:
  /**
   * @brief Binds buffer to the current vertex array
   *
   */
  void Bind() const;
  /**
   * @brief Unbinds buffer to the current vertex array
   *
   */
  void Unbind() const;
  /**
   * @brief Getter to retrieve unique buffer id
   *
   * @return uint64_t const
   */
  uint64_t const ID() const;
  /**
   * @brief Getter to retrieve raw data stored in the buffer
   *
   * @return T const* const
   */
  virtual T const* const GetBufferMemory() const = 0;
  /**
   * @brief Getter to retrieve raw data stored in the buffer
   *
   * @return T const* const
   */
  virtual T* const GetBufferMemory() = 0;
  /**
   * @brief Getter to retrieve size of the buffer
   *
   * @return uint64_t
   */
  virtual uint64_t Size() const = 0;
};

template <GLenum target, typename T>
inline void BufferI<target, T>::CreateBuffer(GLenum purpose)
{
  // Check to ensure that no buffer is bound to current _id
  if (glIsBuffer(_id) == GL_FALSE)
  {
    // Request OpenGL to create a buffer and store handle in the _id
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
  // Ensure that no buffer is bound to current id if the id value is not default
  if (this->_id != UINT32_MAX)
  {
    this->~BufferI();
  }
  // Move objMove._id since possibly OpenGL buffer is bound to it and replace it
  // with default value
  this->_id = std::exchange(objMove._id, UINT32_MAX);
  this->_purpose = std::move(objMove._purpose);
  return *this;
}

template <GLenum target, typename T>
inline BufferI<target, T>::~BufferI()
{
  // Request OpenGL to delete buffer if it is bound to the _id
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
  // Bind buffer to the current vertex array if _id represents an OpenGL buffer
  if (glIsBuffer(_id))
  {
    _(glBindBuffer(target, _id));
  }
}

template <GLenum target, typename T>
inline void BufferI<target, T>::Unbind() const
{
  // Bind buffer under value 0 effectively means unbinding previously bound
  // buffer
  _(glBindBuffer(target, 0));
}
template <GLenum target, typename T>
inline uint64_t const BufferI<target, T>::ID() const
{
  return _id;
}

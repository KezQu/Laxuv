#pragma once

#include <cstdint>
#include <initializer_list>
#include <vector>

#include "BufferI.h"
#include "Debug.h"
#include "GL/glew.h"

/**
 * @brief Class implementing abstract class BufferI which stores buffer values
 * in the CPU memory
 *
 * @tparam target Type of a buffer
 * @tparam T Type of data to be stored inside buffers' memory
 */
template <GLenum target, typename T>
class CPUBuffer : public BufferI<target, T>
{
 protected:
  std::vector<T> _data;

 protected:
  /**
   * @brief Constructs an empty CPU buffer object
   *
   */
  explicit CPUBuffer();
  /**
   * @brief Constructs a CPU buffer with data specified in the initializer_list
   *
   */
  CPUBuffer(std::initializer_list<T> data);
  /**
   * @brief Constructs a CPU buffer with data specified in the vector
   *
   */
  CPUBuffer(std::vector<T>&& data);
  CPUBuffer(CPUBuffer const& objCopy) = delete;
  explicit CPUBuffer(CPUBuffer&& objMove) = default;
  CPUBuffer& operator=(CPUBuffer const& objCopy) = delete;
  CPUBuffer& operator=(CPUBuffer&& objMove) = default;
  ~CPUBuffer() override = default;

 public:
  /**
   * @brief Getter that allows direct read-only access to underlying data stored
   * inside a vector
   *
   * @return std::vector<T> const&
   */
  std::vector<T> const& Data() const;
  /**
   * @brief Getter that allows direct access to underlying data stored inside a
   * vector
   *
   * @return std::vector<T> const&
   */
  std::vector<T>& Data();
  T const* const GetBufferMemory() const override;
  T* const GetBufferMemory() override;
  /**
   * @brief Getter to retrieve number of elements of type T stored in the buffer
   *
   * @return uint64_t
   */
  uint64_t Size() const final;
  /**
   * @brief Method that allows to update data stored inside a buffer memory
   *
   * @param data
   */
  void UpdateBufferMemory(std::vector<T>&& data);
  /**
   * @brief Method that allows to update data stored inside a buffer memory
   *
   * @param data
   */
  void UpdateBufferMemory(std::initializer_list<T> data);
  /**
   * @brief Method that allows to add a single data to an existing buffer
   *
   * @param new_element new data to be added to the buffer
   */
  void AddToBufferMemory(T const& new_element);
  /**
   * @brief Method responsible for removal data at the specified index inside
   * current buffer data
   *
   * @param element_id index of a data to be deleted
   */
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
  // Initializes buffer for a given purpose with a specified size and copy
  // current buffer data to GPU memory
  _(glNamedBufferData(this->_id, Size() * sizeof(T), GetBufferMemory(),
                      this->_purpose));
}

template <GLenum target, typename T>
inline CPUBuffer<target, T>::CPUBuffer(std::vector<T>&& data)
    : BufferI<target, T>{GL_STATIC_DRAW}, _data{data}
{
  // Initializes buffer for a given purpose with a specified size and copy
  // current buffer data to GPU memory
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
  // Exchange buffer data and reinitialize OpenGL buffer with new data
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
  // Add new data to buffer and reinitialize OpenGL buffer with new data
  _data.push_back(new_element);
  _(glNamedBufferData(this->_id, Size() * sizeof(T), GetBufferMemory(),
                      this->_purpose));
}

template <GLenum target, typename T>
inline void CPUBuffer<target, T>::RemoveFromBufferMemory(
    uint32_t const element_id)
{
  // Delete data under specified index and reinitialize OpenGL buffer with new
  // data
  _data.erase(_data.begin() + element_id);
  _(glNamedBufferData(this->_id, Size() * sizeof(T), GetBufferMemory(),
                      this->_purpose));
}

template <GLenum target, typename T>
inline uint64_t CPUBuffer<target, T>::Size() const
{
  return _data.size();
}

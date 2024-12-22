#pragma once

#include "CPUBuffer.h"

/**
 * @brief Class representing buffer for storing indexes
 *
 */
class IndexBuffer : public CPUBuffer<GL_ELEMENT_ARRAY_BUFFER, uint32_t>
{
 public:
  using type = uint32_t;

 public:
  /**
   * @brief Constructs new empty index buffer object
   *
   */
  IndexBuffer() = default;
  /**
   * @brief Constructs new index buffer with specified initial data
   *
   * @param data initial data to be stored inside a buffer
   */
  IndexBuffer(std::initializer_list<type> data)
      : CPUBuffer<GL_ELEMENT_ARRAY_BUFFER, type>(data)
  {
  }
  IndexBuffer(IndexBuffer const& objCopy) = delete;
  IndexBuffer(IndexBuffer&& objMove) = default;
  IndexBuffer& operator=(IndexBuffer const& objCopy) = delete;
  IndexBuffer& operator=(IndexBuffer&& objMove) = default;
  ~IndexBuffer() override = default;
};
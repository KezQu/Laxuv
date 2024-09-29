#pragma once

#include <CPUBuffer.h>

class IndexBuffer : public CPUBuffer<GL_ELEMENT_ARRAY_BUFFER, uint32_t>
{
 public:
  using type = uint32_t;

 public:
  IndexBuffer() = default;
  IndexBuffer(std::initializer_list<type> data);
  IndexBuffer(IndexBuffer const& objCopy) = delete;
  IndexBuffer(IndexBuffer&& objMove) = default;
  IndexBuffer& operator=(IndexBuffer const& objCopy) = delete;
  IndexBuffer& operator=(IndexBuffer&& objMove) = default;
  ~IndexBuffer() override = default;
};
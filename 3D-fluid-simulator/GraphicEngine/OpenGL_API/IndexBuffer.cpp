#include <IndexBuffer.h>

IndexBuffer::IndexBuffer(std::initializer_list<type> data)
    : CPUBuffer<GL_ELEMENT_ARRAY_BUFFER, type>(data)
{
}
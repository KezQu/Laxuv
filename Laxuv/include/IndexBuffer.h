#pragma once

#include <Buffer.h>

template<std::size_t N>
class IndexBuffer : public Buffer<GL_ELEMENT_ARRAY_BUFFER, GLuint, N> {
public:
	IndexBuffer(GLuint const (&data)[N]);
	virtual ~IndexBuffer() override;
};
template<std::size_t N>
inline IndexBuffer<N>::IndexBuffer(GLuint const (&data)[N])
	: Buffer<GL_ELEMENT_ARRAY_BUFFER, GLuint, N>(data)
{}
template<std::size_t N>
inline IndexBuffer<N>::~IndexBuffer()
{}

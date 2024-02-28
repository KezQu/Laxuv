#pragma once

#include <Buffer.h>

class IndexBuffer : public Buffer<GL_ELEMENT_ARRAY_BUFFER, GLuint> {
public:
	using type = GLuint;
	IndexBuffer();
	IndexBuffer(std::initializer_list<type> data);
	IndexBuffer(IndexBuffer const& objCopy) = delete;
	IndexBuffer(IndexBuffer && objMove) = default;
	IndexBuffer& operator=(IndexBuffer const& objCopy) = delete;
	IndexBuffer& operator=(IndexBuffer&& objMove) = default;
	virtual ~IndexBuffer() override;
};
#include <IndexBuffer.h>

IndexBuffer::IndexBuffer()
	:Buffer<GL_ELEMENT_ARRAY_BUFFER, type>()
{}
IndexBuffer::IndexBuffer(std::initializer_list<type> data)
	: Buffer<GL_ELEMENT_ARRAY_BUFFER, type>(data)
{}
IndexBuffer::~IndexBuffer()
{}
 
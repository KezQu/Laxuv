#pragma once

#include <array>
#include <GL/glew.h>

template<GLenum Target, typename T, std::size_t N>
class Buffer {
protected:
	std::array<T,N> _data;
	GLuint _buffer{ 0 };
protected:
	Buffer(T const (&data)[N]);
	virtual ~Buffer();
public:
	void Bind() const;
	void Unbind() const;
};

template<GLenum Target, typename T, std::size_t N>
inline Buffer<Target, T, N>::Buffer(T const (&data)[N])
	:_data(std::to_array(data))
{
	glGenBuffers(1, &_buffer);
	glBindBuffer(Target, _buffer);
	glBufferData(Target, N, _data.data(), GL_STATIC_DRAW);
	glBindBuffer(Target, 0);
}
template<GLenum Target, typename T, std::size_t N>
inline Buffer<Target, T, N>::~Buffer()
{
	glDeleteBuffers(1, &_buffer);
}
template<GLenum Target, typename T, std::size_t N>
inline void Buffer<Target, T, N>::Bind() const {
	glBindBuffer(Target, _buffer);
}
template<GLenum Target, typename T, std::size_t N>
inline void Buffer<Target, T, N>::Unbind() const {
	glBindBuffer(Target, 0);
}

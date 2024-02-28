#pragma once

#include <vector>
#include <GL/glew.h>
#include <Debug.h>

template<GLenum Target, typename T>
class Buffer {
protected:
	std::vector<T> _data;
	GLuint _id{ 0 };
public:
	GLenum target{ Target };
protected:
	Buffer();
	Buffer(std::initializer_list<T> data);
	virtual ~Buffer();
public:
	void Bind() const;
	void Unbind() const;
	void Update(std::initializer_list<T> data);
	void Add(std::initializer_list<T> data);
	std::size_t Size() const;
};

template<GLenum Target, typename T>
inline Buffer<Target, T>::Buffer()
{
	glCreateBuffers(1, &_id);
}

template<GLenum Target, typename T>
inline Buffer<Target, T>::Buffer(std::initializer_list<T> data)
	:_data(data)
{
	glCreateBuffers(1, &_id);
	glNamedBufferData(_id, _data.size(), _data.data(), GL_STATIC_DRAW);
}
template<GLenum Target, typename T>
inline Buffer<Target, T>::~Buffer()
{
	glDeleteBuffers(1, &_id);
}
template<GLenum Target, typename T>
inline void Buffer<Target, T>::Bind() const {
	glBindBuffer(Target, _id);
}
template<GLenum Target, typename T>
inline void Buffer<Target, T>::Unbind() const {
	glBindBuffer(Target, 0);
}
template<GLenum Target, typename T>
inline void Buffer<Target, T>::Update(std::initializer_list<T> data) {
	glNamedBufferData(_id, _data.size(), nullptr, GL_STATIC_DRAW);
	_data.clear();
	_data.insert(std::end(_data), std::begin(data), std::end(data));
	glNamedBufferData(_id, _data.size(), _data.data(), GL_STATIC_DRAW);
}
template<GLenum Target, typename T>
inline void Buffer<Target, T>::Add(std::initializer_list<T> data){
	glNamedBufferData(_id, _data.size(), nullptr, GL_STATIC_DRAW);
	_data.insert(std::end(_data), std::begin(data), std::end(data));
	glNamedBufferData(_id, _data.size(), _data.data(), GL_STATIC_DRAW);
}
template<GLenum Target, typename T>
inline std::size_t Buffer<Target, T>::Size() const {
	return _data.size();
}

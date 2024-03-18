#pragma once

#include <GL/glew.h>
#include <Debug.h>
#include <vector>

template<GLenum Target, typename T>
class Buffer {
protected:
	std::vector<T> _data;
	GLuint _id{ 0 };
public:
	const GLenum target{ Target };
protected:
	Buffer();
	Buffer(std::initializer_list<T> data);
	Buffer(Buffer const& objCopy) = delete;
	Buffer(Buffer&& objMove);
	Buffer& operator=(Buffer const& objCopy) = delete;
	Buffer& operator=(Buffer&& objMove);
	virtual ~Buffer();
public:
	void Bind() const;
	void Unbind() const;
	std::vector<T> const& Data() const;
	T const* const RawData() const;
	void UpdateData(std::vector<T> const& data);
	void UpdateData(std::initializer_list<T> data);
	void Add(std::initializer_list<T> data);
	std::size_t Size() const;
	GLuint const ID() const;
};

template<GLenum Target, typename T>
inline Buffer<Target, T>::Buffer() {
	glCreateBuffers(1, &_id);
}

template<GLenum Target, typename T>
inline Buffer<Target, T>::Buffer(std::initializer_list<T> data)
	:_data(data)
{
	glCreateBuffers(1, &_id);
	glNamedBufferData(_id, _data.size() * sizeof(T), _data.data(), GL_STATIC_DRAW);
}
template<GLenum Target, typename T>
inline Buffer<Target, T>::Buffer(Buffer<Target, T>&& objMove) {
	*this = std::move(objMove);
}
template<GLenum Target, typename T>
inline Buffer<Target, T>& Buffer<Target, T>::operator=(Buffer<Target, T>&& objMove) {
	this->~Buffer();
	_data = std::exchange(objMove._data, {});
	_id = std::exchange(objMove._id, 0);
	return *this;
}

template<GLenum Target, typename T>
inline Buffer<Target, T>::~Buffer()
{
	if (_id != 0) {
		glDeleteBuffers(1, &_id);
	}
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
inline std::vector<T> const& Buffer<Target, T>::Data() const {
	return _data;
}
template<GLenum Target, typename T>
inline T const* const Buffer<Target, T>::RawData() const {
	return _data.data();
}
template<GLenum Target, typename T>
inline void Buffer<Target, T>::UpdateData(std::vector<T> const& data) {
	glNamedBufferData(_id, _data.size() * sizeof(T), nullptr, GL_STATIC_DRAW);
	_data.clear();
	_data.insert(std::end(_data), std::begin(data), std::end(data));
	glNamedBufferData(_id, _data.size() * sizeof(T), _data.data(), GL_STATIC_DRAW);
}
template<GLenum Target, typename T>
inline void Buffer<Target, T>::UpdateData(std::initializer_list<T> data) {
	UpdateData(std::vector<T>(std::begin(data), std::end(data)));
}
template<GLenum Target, typename T>
inline void Buffer<Target, T>::Add(std::initializer_list<T> data){
	glNamedBufferData(_id, _data.size() * sizeof(T), nullptr, GL_STATIC_DRAW);
	_data.insert(std::end(_data), std::begin(data), std::end(data));
	glNamedBufferData(_id, _data.size() * sizeof(T), _data.data(), GL_STATIC_DRAW);
}
template<GLenum Target, typename T>
inline std::size_t Buffer<Target, T>::Size() const {
	return _data.size();
}
template<GLenum Target, typename T>
inline GLuint const Buffer<Target, T>::ID() const {
	return _id;
}

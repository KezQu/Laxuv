#pragma once

#include <GL/glew.h>
#include <Debug.h>
#include <vector>

template<GLenum target, typename T>
class BufferI {
protected:
	uint32_t _id{ 0 };
public:
	const GLenum Target{ target };
protected:
	explicit BufferI();
	BufferI(BufferI const& objCopy) = delete;
	explicit BufferI(BufferI&& objMove);
	BufferI& operator=(BufferI const& objCopy) = delete;
	BufferI& operator=(BufferI&& objMove);
	virtual ~BufferI();
public:
	void Bind() const;
	void Unbind() const;
	uint64_t const ID() const;
	virtual T const* const GetBufferMemory() const = 0;
	virtual T* const GetBufferMemory() = 0;
	virtual uint64_t Size() const = 0;
};

template<GLenum target, typename T>
inline BufferI<target, T>::BufferI(){
	glCreateBuffers(1, &_id);
}

template<GLenum target, typename T>
inline BufferI<target, T>::BufferI(BufferI<target, T>&& objMove)
	:_id{std::exchange(objMove._id, 0U)}
{}
template<GLenum target, typename T>
inline BufferI<target, T>& BufferI<target, T>::operator=(BufferI<target, T>&& objMove) {
	this->~BufferI();
	this->_id = std::exchange(objMove._id, 0U);
	return *this;
}

template<GLenum target, typename T>
inline BufferI<target, T>::~BufferI()
{
	if (_id != 0) {
		glDeleteBuffers(1, &_id);
	}
}
template<GLenum target, typename T>
inline void BufferI<target, T>::Bind() const {
	_(glBindBuffer(target, _id));
}
template<GLenum target, typename T>
inline void BufferI<target, T>::Unbind() const {
	glBindBuffer(target, 0);
}
template<GLenum target, typename T>
inline uint64_t const BufferI<target, T>::ID() const {
	return _id;
}

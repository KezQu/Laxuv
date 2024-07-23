#pragma once

#include "Buffer.h"

template<typename T>
class ShaderStorageBuffer : public Buffer<GL_SHADER_STORAGE_BUFFER, T>{
private:
	int64_t _bindingIndex{ -1 };
public:
	ShaderStorageBuffer();
	ShaderStorageBuffer(std::initializer_list<T> data);
	ShaderStorageBuffer(ShaderStorageBuffer const& objCopy) = delete;
	ShaderStorageBuffer(ShaderStorageBuffer&& objMove);
	ShaderStorageBuffer& operator=(ShaderStorageBuffer const& objCopy) = delete;
	ShaderStorageBuffer& operator=(ShaderStorageBuffer&& objMove);
	~ShaderStorageBuffer() override;
	void Bind() const;
};

template<typename T>
inline ShaderStorageBuffer<T>::ShaderStorageBuffer() {}

template<typename T>
inline ShaderStorageBuffer<T>::ShaderStorageBuffer(std::initializer_list<T> data)
	:Buffer<this->target, T>{data}
{}

template<typename T>
inline ShaderStorageBuffer<T>::ShaderStorageBuffer(ShaderStorageBuffer&& objMove)
{
	*this = std::move(objMove);
}

template<typename T>
inline ShaderStorageBuffer<T>& ShaderStorageBuffer<T>::operator=(ShaderStorageBuffer&& objMove)
{
	this->~ShaderStorageBuffer();
	this->_data = std::exchange(objMove._data, {});
	this->_id = std::exchange(objMove._id, 0);
	_bindingIndex = std::exchange(objMove._bindingIndex, {});
	return *this;
}

template<typename T>
inline ShaderStorageBuffer<T>::~ShaderStorageBuffer()
{}

template<typename T>
inline void ShaderStorageBuffer<T>::Bind() const
{
	Buffer<this->target, T>::Bind();

}

#pragma once

#include <CPUBuffer.h>
#include <GPUBuffer.h>

template<typename T>
class ShaderStorageBuffer : public CPUBuffer<GL_SHADER_STORAGE_BUFFER, T>{
public:
	using BufferType = T;

	explicit ShaderStorageBuffer() = default;
	explicit ShaderStorageBuffer(uint64_t initialBufferSize);
	ShaderStorageBuffer(ShaderStorageBuffer const& objCopy) = delete;
	explicit ShaderStorageBuffer(ShaderStorageBuffer&& objMove);
	ShaderStorageBuffer& operator=(ShaderStorageBuffer const& objCopy) = delete;
	ShaderStorageBuffer& operator=(ShaderStorageBuffer&& objMove);
	~ShaderStorageBuffer() override;

	std::vector<T> GetBufferSubData(uint64_t offset, uint64_t payloadSize);
};

template<typename T>
inline ShaderStorageBuffer<T>::ShaderStorageBuffer(uint64_t initialBufferSize)
	:GPUBuffer<GL_SHADER_STORAGE_BUFFER>{ initialBufferSize }
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
	return *this;
}

template<typename T>
inline ShaderStorageBuffer<T>::~ShaderStorageBuffer()
{}

template<typename T>
inline std::vector<T> ShaderStorageBuffer<T>::GetBufferSubData(uint64_t objectsOffset, uint64_t numberOfObjectsToRead)
{
	std::vector<T> bufferSubData(numberOfObjectsToRead);
	glGetNamedBufferSubData(this->ID(), objectsOffset * sizeof(T), numberOfObjectsToRead * sizeof(T), bufferSubData.data());
	//glGetNamedBufferSubData(this->ID(), 0, 0, bufferSubData.data());
	return bufferSubData;
}

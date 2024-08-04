#pragma once

#include <CPUBuffer.h>
#include <GPUBuffer.h>

template<typename T>
class ShaderStorageBuffer : public GPUBuffer<GL_SHADER_STORAGE_BUFFER, T>{
public:
	using BufferType = T;

	explicit ShaderStorageBuffer() = default;
	explicit ShaderStorageBuffer(uint64_t initialBufferSize);
	ShaderStorageBuffer(ShaderStorageBuffer const& objCopy) = delete;
	explicit ShaderStorageBuffer(ShaderStorageBuffer&& objMove) = default;
	ShaderStorageBuffer& operator=(ShaderStorageBuffer const& objCopy) = delete;
	ShaderStorageBuffer& operator=(ShaderStorageBuffer&& objMove) = default;
	~ShaderStorageBuffer() override = default;

	std::vector<T> GetBufferSubData(uint64_t offset, uint64_t payloadSize);
};

template<typename T>
inline ShaderStorageBuffer<T>::ShaderStorageBuffer(uint64_t initialBufferSize)
	:GPUBuffer<GL_SHADER_STORAGE_BUFFER, T>{ initialBufferSize }
{}

template<typename T>
inline std::vector<T> ShaderStorageBuffer<T>::GetBufferSubData(uint64_t objectsOffset, uint64_t numberOfObjectsToRead)
{
	std::vector<T> bufferSubData(numberOfObjectsToRead);
	_(glGetNamedBufferSubData(this->ID(), objectsOffset * sizeof(T), numberOfObjectsToRead * sizeof(T), bufferSubData.data()));
	//glGetNamedBufferSubData(this->ID(), 0, 0, bufferSubData.data());
	return bufferSubData;
}

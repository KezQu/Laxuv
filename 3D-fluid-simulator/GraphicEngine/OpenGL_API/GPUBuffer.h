#pragma once

#include <BufferI.h>

template<GLenum target, typename T = void>
class GPUBuffer : public BufferI<target, T> {
protected:
	uint64_t _bufferSize;
	uint32_t _id{ 0 };
public:
	const GLenum Target{ target };
protected:
	explicit GPUBuffer() = default;
	explicit GPUBuffer(uint64_t initialBufferSize);
	GPUBuffer(GPUBuffer const& objCopy) = delete;
	explicit GPUBuffer(GPUBuffer&& objMove);
	GPUBuffer& operator=(GPUBuffer const& objCopy) = delete;
	GPUBuffer& operator=(GPUBuffer&& objMove);
	~GPUBuffer() override = default;
public:
	T const* const GetBufferMemory() const override;
	T* const GetBufferMemory() override;
	uint64_t Size() const final;
	void SetBufferMemorySize(uint64_t newBufferMemorySize);
};
template<GLenum target, typename T>
inline GPUBuffer<target, T>::GPUBuffer(uint64_t initialBufferSize)
	:_bufferSize{initialBufferSize}
{
	glNamedBufferData(this->_id, Size() * sizeof(T), GetBufferMemory(), GL_STATIC_DRAW);
}
template<GLenum target, typename T>
inline GPUBuffer<target, T>::GPUBuffer(GPUBuffer<target, T>&& objMove)
	: BufferI<target, T>(std::move(objMove)),
	_bufferSize{ std::exchange(objMove._bufferSize, {}) }
{}
template<GLenum target, typename T>
inline GPUBuffer<target, T>& GPUBuffer<target, T>::operator=(GPUBuffer<target, T>&& objMove) {
	this->~GPUBuffer();
	this->_bufferSize = std::exchange(objMove._bufferSize, {});
	this->_id = std::exchange(objMove._id, 0U);
	return *this;
}
template<GLenum target, typename T>
inline T const* const GPUBuffer<target, T>::GetBufferMemory() const {
	return nullptr;
}
template<GLenum target, typename T>
inline T* const GPUBuffer<target, T>::GetBufferMemory() {
	return nullptr;
}
template<GLenum target, typename T>
inline uint64_t GPUBuffer<target, T>::Size() const {
	return _bufferSize;
}

template<GLenum target, typename T>
inline void GPUBuffer<target, T>::SetBufferMemorySize(uint64_t newBufferMemorySize)
{
	_bufferSize = newBufferMemorySize;
	glNamedBufferData(this->_id, Size() * sizeof(T), GetBufferMemory(), GL_STATIC_DRAW);
}

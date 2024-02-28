#pragma once

#include <Buffer.h>
#include <type_traits>

template<typename T, GLuint S>
class VertexBuffer : public Buffer<GL_ARRAY_BUFFER, T>{
public:
	using type = T;
	GLuint size = S;
	GLint _bindingIndex{ -1 };
public:
	VertexBuffer();
	VertexBuffer(std::initializer_list<type> data);
	VertexBuffer(VertexBuffer const& objCopy) = delete;
	VertexBuffer(VertexBuffer && objMove) = default;
	VertexBuffer& operator=(VertexBuffer const& objCopy) = delete;
	VertexBuffer& operator=(VertexBuffer&& objMove) = default;
	virtual ~VertexBuffer() override;
	void DefineAttribute(GLuint const vertexArrayID, GLuint const bindingIndex);
	void EnableAttribute(GLuint const vertexArrayID) const;
	void DisableAttribute(GLuint const vertexArrayID) const;
private:
	template<typename TypeTest = type, std::enable_if_t<std::is_same<GLfloat, TypeTest>::value, bool> = true >
	GLenum CheckType() const { return GL_FLOAT; }
	template<typename TypeTest = type, std::enable_if_t<std::is_same<GLuint, TypeTest>::value, bool> = true >
	GLenum CheckType() const { return GL_UNSIGNED_INT; }
	template<typename TypeTest = type, std::enable_if_t<std::is_same<GLubyte, TypeTest>::value, bool> = true >
	GLenum CheckType() const { return GL_UNSIGNED_BYTE; }
};

template<typename T, GLuint S>
inline VertexBuffer<T, S>::VertexBuffer()
	:Buffer<GL_ARRAY_BUFFER, type>()
{}
template<typename T, GLuint S>
inline VertexBuffer<T, S>::VertexBuffer(std::initializer_list<type> data)
	: Buffer<GL_ARRAY_BUFFER, type>(data)
{}
template<typename T, GLuint S>
inline VertexBuffer<T, S>::~VertexBuffer()
{}
template<typename T, GLuint S>
void inline VertexBuffer<T, S>::DefineAttribute(GLuint const vertexArrayID, GLuint const bindingIndex){
	_bindingIndex = bindingIndex;
	glVertexArrayVertexBuffer(vertexArrayID, _bindingIndex, this->_id, 0, sizeof(type));
	glVertexArrayAttribFormat(vertexArrayID, _bindingIndex, size, CheckType(), GL_FALSE, 0);
	glVertexArrayAttribBinding(vertexArrayID, _bindingIndex, bindingIndex);
}
template<typename T, GLuint S>
void inline VertexBuffer<T, S>::EnableAttribute(GLuint const vertexArrayID) const {
	glEnableVertexArrayAttrib(vertexArrayID, _bindingIndex);

}
template<typename T, GLuint S>
void inline VertexBuffer<T, S>::DisableAttribute(GLuint const vertexArrayID) const {
	glDisableVertexArrayAttrib(vertexArrayID, _bindingIndex);
}
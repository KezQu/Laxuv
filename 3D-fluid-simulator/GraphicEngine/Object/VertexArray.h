#pragma once

#include <IndexBuffer.h>
#include <VertexBuffer.h>
#include <glm/matrix.hpp>

struct VertexBufferData {
	IndexBuffer indexBuffer;
	VertexBuffer<GLfloat, 3> coordinateBuffer;
	VertexBuffer<GLubyte, 4> colorBuffer;
};
struct Vertex {
	glm::vec3 coordinate;
	glm::uvec4 color{ 127, 127, 127, 127 };
};
class VertexArray {
public:
	using type = VertexBufferData;
private:
	type _data;
	GLuint _id{ 0 };
public:
	VertexArray();
	VertexArray(type&& vertexBufferData);
	VertexArray(std::initializer_list<Vertex> vertices, IndexBuffer&& idxBuffer);
	VertexArray(VertexArray const& objCopy) = delete;
	VertexArray(VertexArray&& objMove);
	VertexArray& operator=(VertexArray const& objCopy) = delete;
	VertexArray& operator=(VertexArray&& objMove);
	~VertexArray();
	void Bind() const;
	void Unbind() const;
	std::size_t Size() const;
	GLenum IndexBufferType() const;
	GLuint const ID() const;
};

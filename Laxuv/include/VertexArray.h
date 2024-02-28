#pragma once

#include <IndexBuffer.h>
#include <VertexBuffer.h>

struct VertexBufferData {
	IndexBuffer indexBuffer;
	VertexBuffer<GLfloat, 3> coordinateBuffer;
	VertexBuffer<GLubyte, 4> colorBuffer;
};
struct Vertex {
	GLuint indice;
	GLfloat coordinate[3];
	GLubyte color[4];
};
class VertexArray {
private:
	using type = VertexBufferData;
	type _data;
	GLuint _id;
public:
	VertexArray();
	VertexArray(type && vertices);
	~VertexArray();
	void Bind() const;
	void Unbind() const;
	void AppendVertex(Vertex const& vertex);
};

#include <VertexArray.h>

VertexArray::VertexArray()
{
	glCreateVertexArrays(1, &_id);
}

VertexArray::VertexArray(type && vertices)
	:_data{ std::exchange(vertices, {}) }
{
	glCreateVertexArrays(1, &_id);
	_data.coordinateBuffer.DefineAttribute(_id, 0);
	_data.colorBuffer.DefineAttribute(_id, 1);
}

VertexArray::~VertexArray()
{
	glDeleteVertexArrays(1, &_id);
}

void VertexArray::Bind() const
{
	glBindVertexArray(_id);
	_data.indexBuffer.Bind();
	_data.colorBuffer.EnableAttribute(_id);
	_data.coordinateBuffer.EnableAttribute(_id);
}

void VertexArray::Unbind() const
{
	_data.coordinateBuffer.DisableAttribute(_id);
	_data.colorBuffer.DisableAttribute(_id);
	_data.indexBuffer.Unbind();
	glBindVertexArray(0);
}

void VertexArray::AppendVertex(Vertex const& vertex)
{
}

#include <VertexArray.h>

VertexArray::VertexArray()
	:_id{ 0 }
{
	glCreateVertexArrays(1, &_id);
	_data.coordinateBuffer.DefineAttribute(_id, 0);
	_data.colorBuffer.DefineAttribute(_id, 1);
}

VertexArray::VertexArray(type && vertexBufferData)
	:VertexArray()
{
	_data = std::exchange(vertexBufferData, {});
}
VertexArray::VertexArray(std::initializer_list<Vertex> vertices, IndexBuffer&& idxBuffer)
	:VertexArray()
{
	std::vector<GLfloat> tmpCoordinates;
	std::vector<GLubyte> tmpColors;
	for (auto& vertex : vertices) {
		for (int i = 0; i < 3; i++) tmpCoordinates.push_back(vertex.coordinate[i]);
		for (int i = 0; i < 4; i++) tmpColors.push_back(vertex.color[i]);
	}
	_data.indexBuffer = std::move(idxBuffer);
	_data.coordinateBuffer.UpdateData(std::move(tmpCoordinates));
	_data.colorBuffer.UpdateData(std::move(tmpColors));
}
VertexArray::VertexArray(VertexArray&& objMove) {
	*this = std::move(objMove);
}
VertexArray& VertexArray::operator=(VertexArray&& objMove) {
	this->~VertexArray();
	_data = std::exchange(objMove._data, {});
	_id = std::exchange(objMove._id, 0);
	return *this;
}

VertexArray::~VertexArray()
{
	if (_id != 0) {
		glDeleteVertexArrays(1, &_id);
	}
}
VertexBufferData const& VertexArray::Data() const {
	return _data;
}

void VertexArray::Bind() const
{
	_(glBindVertexArray(_id));
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
std::size_t VertexArray::Size() const {
	return _data.indexBuffer.Size();
}
GLenum VertexArray::IndexBufferType() const {
	return GL_UNSIGNED_INT;
}
GLuint const VertexArray::ID() const {
	return _id;
}

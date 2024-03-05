#include <Object.h>

Object::Object(VertexArray&& vertexArray)
	:_vertexArray{ std::move(vertexArray) }
{}
Object::Object(Program&& renderer, VertexArray&& vertexArray)
	:_vertexArray{ std::move(vertexArray) },
	_renderer{ std::move(renderer) }
{}
Object::Object(Program&& renderer)
	:_renderer{ std::move(renderer) }
{}
void Object::Draw()
{
	_vertexArray.Bind();
	if (!_renderer.isLinked())
		_renderer.Link();
	_renderer.Bind();
	glDrawElements(GL_TRIANGLES, _vertexArray.Size(), _vertexArray.IndexBufferType(), nullptr);
	_renderer.Unbind();
	_vertexArray.Unbind();
}

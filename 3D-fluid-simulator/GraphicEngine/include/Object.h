#pragma once

#include <Program.h>
#include <VertexArray.h>

class Object {
protected:
	VertexArray _vertexArray;
	Program _renderer;
protected:
	Object(Program&& renderer, VertexArray&& vertexArray);
	Object(VertexArray&& vertexArray);
	Object(Program&& renderer);
public:
	void Draw();
};

#include <Square.h>

Square::Square() 
	:Square(
		{ {-1.f, 1.f, 0.0f} },
		{ {1.f, 1.f, 0.0f} },
		{ {-1.f, -1.f, 0.0f} },
		{ {1.f, -1.f, 0.0f} })
{}
Square::Square(Vertex center, float edgeLength)
	:Square(
		{ {center.coordinate[0] - edgeLength / 2.f, center.coordinate[1] + edgeLength / 2.f, center.coordinate[2]} },
		{ {center.coordinate[0] + edgeLength / 2.f, center.coordinate[1] + edgeLength / 2.f, center.coordinate[2]} },
		{ {center.coordinate[0] - edgeLength / 2.f, center.coordinate[1] - edgeLength / 2.f, center.coordinate[2]} },
		{ {center.coordinate[0] + edgeLength / 2.f, center.coordinate[1] - edgeLength / 2.f, center.coordinate[2]} })
{}
Square::Square(Vertex v0, Vertex v1, Vertex v2, Vertex v3)
	:Object<GL_TRIANGLES>(Program{
		{ GL_VERTEX_SHADER, "\\shaders\\CalculateNDC.glsl" },
		{ GL_VERTEX_SHADER, "\\shaders\\Object.vert" },
		{ GL_FRAGMENT_SHADER, "\\shaders\\Object.frag" } },
		VertexArray{ {v0, v1, v2, v3}, {0, 1, 2, 1, 3, 2} })
{}

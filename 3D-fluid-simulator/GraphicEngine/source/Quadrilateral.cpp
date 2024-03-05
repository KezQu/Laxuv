#include <Quadrilateral.h>

Quadrilateral::Quadrilateral() 
	:Quadrilateral(
		{ {-0.5f, 0.5f, 0.0f}, {127, 127, 127, 255} },
		{ {0.5f, 0.5f, 0.0f}, {127, 127, 127, 255} },
		{ {0.5f, -0.5f, 0.0f}, {127, 127, 127, 255} },
		{ {-0.5f, -0.5f, 0.0f}, {127, 127, 127, 255} })
{}

Quadrilateral::Quadrilateral(Vertex v0, Vertex v1, Vertex v2, Vertex v3)
	:Object(Program{ 
		{ GL_VERTEX_SHADER, "\\shaders\\quadrilateral.vert" },
		{ GL_FRAGMENT_SHADER, "\\shaders\\quadrilateral.frag" } },
		VertexArray{ {v0, v1, v2, v3}, {0, 1, 3, 1, 2, 3} })
{}

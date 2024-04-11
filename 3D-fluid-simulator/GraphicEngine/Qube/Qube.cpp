#include <Qube.h>

Qube::Qube()
	:Qube({ { 0, 0, 0 } }, 300)
{}
Qube::Qube(Vertex center, float edgeLength)
	:Qube(
		{ { center.coordinate.x - edgeLength / 2.f, center.coordinate.y + edgeLength / 2.f, center.coordinate.z - edgeLength / 2.f }, center.color },
		{ { center.coordinate.x + edgeLength / 2.f, center.coordinate.y + edgeLength / 2.f, center.coordinate.z - edgeLength / 2.f }, center.color },
		{ { center.coordinate.x - edgeLength / 2.f, center.coordinate.y + edgeLength / 2.f, center.coordinate.z + edgeLength / 2.f }, center.color },
		{ { center.coordinate.x + edgeLength / 2.f, center.coordinate.y + edgeLength / 2.f, center.coordinate.z + edgeLength / 2.f }, center.color },
		{ { center.coordinate.x - edgeLength / 2.f, center.coordinate.y - edgeLength / 2.f, center.coordinate.z - edgeLength / 2.f }, center.color },
		{ { center.coordinate.x + edgeLength / 2.f, center.coordinate.y - edgeLength / 2.f, center.coordinate.z - edgeLength / 2.f }, center.color },
		{ { center.coordinate.x - edgeLength / 2.f, center.coordinate.y - edgeLength / 2.f, center.coordinate.z + edgeLength / 2.f }, center.color },
		{ { center.coordinate.x + edgeLength / 2.f, center.coordinate.y - edgeLength / 2.f, center.coordinate.z + edgeLength / 2.f }, center.color })
{
	_center = center.coordinate;
}
Qube::Qube(Vertex v1, Vertex v2, Vertex v3, Vertex v4, Vertex v5, Vertex v6, Vertex v7, Vertex v8)
	:Object(VertexArray({ v1,v2,v3,v4,v5,v6,v7,v8 }, 
			{ 0,1,2,1,3,2,
			  5,4,6,7,5,6,
			  1,0,4,5,1,4,
			  2,3,6,3,7,6,
			  0,2,4,2,6,4,
			  3,1,7,1,5,7}), true)
{
	_center = v1.coordinate + v2.coordinate + v3.coordinate + v4.coordinate +
			  v5.coordinate + v6.coordinate + v7.coordinate + v8.coordinate;
}

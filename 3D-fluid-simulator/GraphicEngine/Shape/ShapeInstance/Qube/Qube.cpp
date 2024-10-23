#include <Qube.h>

Qube::Qube(Vertex center, float radius, bool enableTess)
    : Qube({{center.coordinate.x - 1.f, center.coordinate.y + 1.f,
             center.coordinate.z - 1.f},
            center.color},
           {{center.coordinate.x + 1.f, center.coordinate.y + 1.f,
             center.coordinate.z - 1.f},
            center.color},
           {{center.coordinate.x - 1.f, center.coordinate.y + 1.f,
             center.coordinate.z + 1.f},
            center.color},
           {{center.coordinate.x + 1.f, center.coordinate.y + 1.f,
             center.coordinate.z + 1.f},
            center.color},
           {{center.coordinate.x - 1.f, center.coordinate.y - 1.f,
             center.coordinate.z - 1.f},
            center.color},
           {{center.coordinate.x + 1.f, center.coordinate.y - 1.f,
             center.coordinate.z - 1.f},
            center.color},
           {{center.coordinate.x - 1.f, center.coordinate.y - 1.f,
             center.coordinate.z + 1.f},
            center.color},
           {{center.coordinate.x + 1.f, center.coordinate.y - 1.f,
             center.coordinate.z + 1.f},
            center.color},
           radius, enableTess)
{
}

Qube::Qube(Vertex v1, Vertex v2, Vertex v3, Vertex v4, Vertex v5, Vertex v6,
           Vertex v7, Vertex v8, float radius, bool enableTess)
    : Shape(VertexArray({v1, v2, v3, v4, v5, v6, v7, v8},
                        {0, 1, 2, 1, 3, 2, 5, 4, 6, 7, 5, 6, 1, 0, 4, 5, 1, 4,
                         2, 3, 6, 3, 7, 6, 0, 2, 4, 2, 6, 4, 3, 1, 7, 1, 5, 7}),
            radius, enableTess)
{
  auto& coordBufferData = _vertexArray.Data().coordinateBuffer.Data();
}

Essentials::DistributionShape Qube::GetParticleDistribution()
{
  return Essentials::DistributionShape::QUBE;
}

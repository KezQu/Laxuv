#version 460 core

uniform uint DistributionShape = 0;
uniform ivec3 MeshSize;

const float PI = 3.14159265358979323846264338327950288;

vec4 Line(in uint idx, in uint maxParticles)
{
  return vec4(0.f, idx, 0.f, 0.f);
}

vec4 Square(in uint idx, in uint maxParticles)
{
  float meshSize2D = sqrt(maxParticles);
  return vec4(floor(idx / meshSize2D), mod(idx, meshSize2D), 0.f, 0.f);
}

vec4 Cube(in uint idx, in uint maxParticles)
{
  return vec4(mod(idx, MeshSize.x), mod(idx / MeshSize.x, MeshSize.y),
              idx / (MeshSize.x * MeshSize.y), 0.f);
}

const uint UNDEFINED = 0;
const uint D_LINE = 1;
const uint D_SQUARE = 2;
const uint D_CUBE = 3;

vec3 InitDefaultShape(in uint idx, in uint maxParticles)
{
  vec4 positionCandidate = vec4(0);
  switch (DistributionShape)
  {
    case D_LINE:
      positionCandidate = Line(idx, maxParticles);
      break;
    case D_SQUARE:
      positionCandidate = Square(idx, maxParticles);
      break;
    case D_CUBE:
      positionCandidate = Cube(idx, maxParticles);
      break;
    default:
      break;
  }
  return positionCandidate.xyz;
}
#version 460 core

// Specify location of the input from the currently bound VAO
layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec4 inColorVert;

layout(location = 0) out vec4 outColorVert;
layout(location = 1) out vec3 normal;

const uint MaxNeighbors = 512;
const uint NONE = 0;

uniform uint colorType;

// Define struct matching C++ implementation for particle properties for further
// using it for SSBO
struct ParticleProperties
{
  vec4 velocityDFSPHfactor;
  vec4 position;
  vec4 MassDensityPressureDro_Dt;
  vec4 color;
  uint neighbors[MaxNeighbors];
};

// Specify binding point defining properties of the currently used entity with
// std430 memory alignment to match one used in C++
layout(std430, binding = 0) buffer dataBuffer
{
  ParticleProperties particle[];
};

// Declare helper functions for position processing
vec4 CalculateNDC(in vec3 position, in vec3 offset);
vec3 CalculateNormal(in vec3 position);

void main()
{
  // Specify color buffer values based on the chosen color type
  if (colorType == NONE)
  {
    outColorVert = inColorVert / 255.;
  }
  else
  {
    outColorVert = particle[gl_InstanceID].color;
  }
  // Create normal vector for a current vertex for lightning processing
  normal = CalculateNormal(inPosition);
  // Transform vertex position to the NDC
  gl_Position = CalculateNDC(inPosition, particle[gl_InstanceID].position.xyz);
  gl_PointSize = 10;
}
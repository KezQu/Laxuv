#version 460 core

// Specify location of the input from the currently bound VAO
layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec4 inColorVert;

layout(location = 0) out vec4 outColorVert;
layout(location = 3) out vec3 outShapeOffset;

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
  // Specify offset for the instanced rendering of the particles
  outShapeOffset = particle[gl_InstanceID].position.xyz;
  // Forward vertex position using GLSL built-in variable
  gl_Position = vec4(inPosition, 0);
  gl_PointSize = 10;
}
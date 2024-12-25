#version 460 core

// Define specification for the primitive generator
layout(triangles, equal_spacing, ccw) in;
layout(location = 0) in vec4 inColorTese[];
layout(location = 3) in vec3 inShapeOffset[];

layout(location = 0) out vec4 outColorTese;
layout(location = 1) out vec3 normal;

uniform vec3 center;
// Declare helper functions used in this pipeline stage
vec4 CalculateNDC(in vec3 position, in vec3 offset);
vec3 CalculateNormal(in vec3 position);

void main()
{
  vec3 tessVertexCoord = vec3(0.0);
  outColorTese = inColorTese[0];
  // Transform tessellated abstract patch to the model coordinates
  for (int i = 0; i < gl_PatchVerticesIn; i++)
  {
    tessVertexCoord += gl_TessCoord[i] * gl_in[i].gl_Position.xyz;
  }
  // Define the radius of hte sphere to be generated from the vertices
  float R = distance(gl_in[0].gl_Position.xyz, center);
  float tessVertexCenterLength = distance(tessVertexCoord, center);
  // Adjust tessellated vertex position to be part of the surface of a sphere
  // with radius R
  tessVertexCoord = tessVertexCoord + (R - tessVertexCenterLength) *
                                          normalize(tessVertexCoord - center);
  // Calculate normal for the created vertex
  normal = CalculateNormal(tessVertexCoord);
  // Transform vertex to the Normalized Device Coordinates
  gl_Position = CalculateNDC(tessVertexCoord, inShapeOffset[0]);
}
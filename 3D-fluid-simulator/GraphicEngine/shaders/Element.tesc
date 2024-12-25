#version 460 core

// Define incoming data
layout(location = 0) in vec4 inColorTesc[];
layout(location = 3) in vec3 inShapeOffset[];

// Define default number of vertices for further tesselation processing
layout(vertices = 3) out;
layout(location = 0) out vec4 outColorTesc[];
layout(location = 3) out vec3 outShapeOffset[];

uniform uint subdivision;

void main()
{
  // Forward data to the next processing stage
  outColorTesc[gl_InvocationID] = inColorTesc[gl_InvocationID];
  outShapeOffset[gl_InvocationID] = inShapeOffset[gl_InvocationID];
  // Define the amount of the tesselation to be done by the primitive generator
  gl_TessLevelOuter[gl_InvocationID] = subdivision;
  gl_TessLevelInner[0] = subdivision;
  gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;
  barrier();
}
#version 460 core

const float worldScale = 1;

// Define all uniforms needed to correctly transform vertices to worlds
// coordinates
uniform mat4 model;
uniform vec3 scale;
uniform mat4 view;
uniform mat4 projection;
uniform float shapeRadius;

// Helper function transforming given position to the Normalized Device
// Coordinates (NDC)
vec4 CalculateNDC(in vec3 position, in vec3 offset)
{
  vec4 outPosition = vec4(
      (position + offset * worldScale) * scale * worldScale * shapeRadius, 1.0);

  outPosition = projection * view * model * outPosition;

  return outPosition;
}

// Helper function providing basic mechanism for calculating surface normal to
// further lightning processing
vec3 CalculateNormal(in vec3 position)
{
  const mat4 translate_mat = mat4(1, 0, 0, model[3].x, 0, 1, 0, model[3].y, 0,
                                  0, 1, model[3].z, 0, 0, 0, 1);
  vec4 normal =
      transpose(transpose(model) * inverse(translate_mat)) * vec4(position, 1);
  return -normalize(normal.xyz);
}

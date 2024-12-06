#version 460 core

layout(location = 0) in vec4 inColorFrag;
layout(location = 1) in vec3 normal;

out vec4 BaseColor;

const uint NONE = 0;

uniform vec3 ambientColor = vec3(1.0);
uniform vec3 diffuseDirection = vec3(1.0);
uniform vec3 diffuseColor = vec3(0.0);
uniform uint colorType;

void main()
{
  if (colorType != NONE)
  {
    float diffuseStrength = 0;
    if (length(diffuseDirection) != 0)
    {
      diffuseStrength = max(dot(normalize(diffuseDirection), normal), 0.0);
    }
    BaseColor =
        (diffuseStrength * vec4(diffuseColor, 1.0) + vec4(ambientColor, 1.0)) *
        inColorFrag;
  }
  else
  {
    BaseColor = inColorFrag;
  }
}

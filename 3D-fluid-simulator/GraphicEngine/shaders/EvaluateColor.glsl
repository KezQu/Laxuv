#version 460 core

const uint MaxNeighbours = 512;
const uint NONE = 0;

const uint CUSTOM = 1;
const uint VELOCITY = 2;
const uint DENSITY_ERROR = 3;
const uint DIVERGENCE_ERROR = 4;
const uint PRESSURE = 5;

uniform uint colorType;
uniform vec4 color;

struct ParticleProperties
{
  vec4 velocityDFSPHfactor;
  vec4 position;
  vec4 MassDensityPressureDro_Dt;
  vec4 color;
  uint neighbours[MaxNeighbours];
};

layout(std430, binding = 0) buffer dataBuffer
{
  restrict ParticleProperties particle[];
};

vec4 CalculateColor(float property_value)
{
  vec4 chosen_color = color;
  const float boundCheck = 0.25f;

  if (property_value <= -1.f)
  {
    chosen_color.xyz = vec3(1);
  }
  else if (property_value < 0.f)
  {
    chosen_color.xyz = vec3(0, 0, 1 - property_value);
  }
  else if (property_value < boundCheck)
  {
    chosen_color.xyz = vec3(0, property_value * 4, 1);
  }
  else if (property_value < 2 * boundCheck)
  {
    chosen_color.xyz = vec3(0, 1, 1 - (property_value - boundCheck) * 4);
  }
  else if (property_value < 3 * boundCheck)
  {
    chosen_color.xyz = vec3((property_value - 2 * boundCheck) * 4, 1, 0);
  }
  else if (property_value <= 4 * boundCheck)
  {
    chosen_color.xyz = vec3(1, 1 - (property_value - 3 * boundCheck) * 4, 0);
  }
  else if (property_value <= 5 * boundCheck)
  {
    chosen_color.xyz = vec3(1 - (property_value - 4 * boundCheck) * 4, 0, 0);
  }
  else
  {
    chosen_color.xyz = vec3(0);
  }
  return chosen_color;
}

float GetInternalDensity(uint index_x);

vec4 ChooseColor(uint index_x)
{
  const float max_speed = 2e+3;
  const float density0 = GetInternalDensity(index_x);
  vec4 chosen_color = vec4(0);
  switch (colorType)
  {
    case CUSTOM:
      chosen_color = color;
      break;
    case VELOCITY:
      chosen_color = CalculateColor(
          length(particle[index_x].velocityDFSPHfactor.xyz) / max_speed);
      break;
    case DENSITY_ERROR:
      chosen_color = CalculateColor(
          abs(density0 - particle[index_x].MassDensityPressureDro_Dt.y) / particle[index_x].MassDensityPressureDro_Dt.x
          );
      break;
    case DIVERGENCE_ERROR:
      chosen_color = CalculateColor(
          1e-2 * sqrt(abs(particle[index_x].MassDensityPressureDro_Dt.w)) / particle[index_x].MassDensityPressureDro_Dt.x);
      break;
    case PRESSURE:
      chosen_color = CalculateColor(
          1e-2 * sqrt(abs(particle[index_x].MassDensityPressureDro_Dt.z)) / particle[index_x].MassDensityPressureDro_Dt.y);
      break;
  }
  return chosen_color;
}

#pragma once

#include "Camera.h"
#include "Essentials.h"
#include "ProgramDispatch.h"
#include "Simulator.h"
#include "Uniform.h"
#include "VertexArray.h"

struct
{
  glm::vec3 AmbientLightColor{.2f};
  struct
  {
    glm::vec3 color{1.f};
    glm::vec3 direction{-0.3f, -0.2f, -1.f};
  } DiffuseLight;
} LIGHT;

template <GLenum Prim>
class Shape
{
  // TODO: Move Properties to standalone struct
 protected:
  VertexArray _vertexArray;
  Uniform<glm::vec3, float> _location{
      glm::vec3{0.f}, "location",
      ValueProperties{-1200.f, 1200.f, 1.f, "%.1f mm"}};
  Uniform<glm::vec3, float> _scale{glm::vec3{1.f}, "scale",
                                   ValueProperties{0.f, 100.f, 1.f, "%.1f"}};
  Uniform<glm::vec3, float> _rotation{
      glm::vec3{0.f}, "rotation",
      ValueProperties{-360.f, 360.f, 1.f, "%.1f deg"}};
  Uniform<glm::vec3, float> _center{
      glm::vec3{0.f}, "center",
      ValueProperties{-1200.f, 1200.f, 1.f, "%.1f mm"}};
  Uniform<uint32_t> _subdivision{5U, "subdivision", ValueProperties{1U, 50U}};
  Uniform<float> _shapeRadius{1.f, "shapeRadius",
                              ValueProperties{0.1f, 600.f, 1.f, "%.1f mm"}};
  std::pair<Uniform<uint32_t>, Uniform<float>> _shape_color{
      {static_cast<uint32_t>(Essentials::ColorProperty::NONE), "colorType"},
      {0.5f, "colorOpacity", ValueProperties{0.f, 1.f, 1.f, "%.2f"}}};
  bool _enableTesselation{false};
  bool _enableLight{true};
  GLenum _primitiveType{Prim};

 public:
  Shape(VertexArray&& vertexArray = {}, float shapeRadius = 10.f,
        bool enableTess = false);
  Shape(Shape const& obj_copy) = delete;
  Shape(Shape&& obj_move) = default;
  Shape& operator=(Shape const& obj_copy) = delete;
  Shape& operator=(Shape&& obj_move) = default;
  virtual ~Shape() = default;
  std::pair<Uniform<uint32_t>, Uniform<float>>& GetColor();
  Uniform<glm::vec3, float>& GetLocation();
  Uniform<glm::vec3, float>& GetScale();
  Uniform<glm::vec3, float>& GetRotate();
  Uniform<glm::mat4, float> Model() const;
  void Bind() const;
  GLenum GetDrawPrimitive() const;
  VertexArray const& GetVA() const;
  Uniform<glm::vec3, float> const& GetCenter() const;
  bool& EnableTesselation();
  void EnableLight(bool light);
  Uniform<uint32_t>& GetSubdivision();
  Uniform<float>& GetRadius();
  virtual Essentials::DistributionShape GetParticleDistribution() = 0;
};

template <GLenum Prim>
Shape<Prim>::Shape(VertexArray&& vertexArray, float shapeRadius,
                   bool enableTess)
    : _vertexArray{std::move(vertexArray)}, _enableTesselation{enableTess}
{
  _shapeRadius = shapeRadius;
  if (_enableTesselation == true)
  {
    _primitiveType = GL_PATCHES;
  }
  auto& coordBuffer = GetVA().Data().coordinateBuffer;

  for (int i = 0; i < coordBuffer.Size(); i += 3)
  {
    _center += glm::vec3(coordBuffer.Data()[i + 0], coordBuffer.Data()[i + 1],
                         coordBuffer.Data()[i + 2]);
  }
  _center /= static_cast<float>(coordBuffer.Size() / coordBuffer.AttribSize());
}

template <GLenum Prim>
void Shape<Prim>::Bind() const
{
  _vertexArray.Bind();

  Program& renderer =
      _enableTesselation == true
          ? ProgramDispatch::GetInstance().GetTesselationPipeline()
          : ProgramDispatch::GetInstance().GetSimplePipeline();

  if (!renderer.isLinked()) renderer.Link();
  renderer.Bind();

  Simulator::GetInstance().BindUniforms(renderer.ID());

  Model().MapUniform(renderer.ID());
  Camera::GetCamera().View().MapUniform(renderer.ID());
  Camera::GetCamera().Projection().MapUniform(renderer.ID());
  Camera::GetCamera().Viewport().MapUniform(renderer.ID());
  _center.MapUniform(renderer.ID());
  _subdivision.MapUniform(renderer.ID());

  _shape_color.first.MapUniform(renderer.ID());
  _shape_color.second.MapUniform(renderer.ID());
  if (_enableLight)
  {
    auto const ambient_light =
        Uniform<glm::vec3, float>(LIGHT.AmbientLightColor, "ambientLightColor");
    auto const diffuse_light_color = Uniform<glm::vec3, float>(
        LIGHT.DiffuseLight.color, "diffuseLightColor");
    auto const diffuse_light_dir = Uniform<glm::vec3, float>(
        LIGHT.DiffuseLight.direction, "diffuseLightDirection");
    ambient_light.MapUniform(renderer.ID());
    diffuse_light_color.MapUniform(renderer.ID());
    diffuse_light_dir.MapUniform(renderer.ID());
  }
  _shapeRadius.MapUniform(renderer.ID());
}
template <GLenum Prim>
inline std::pair<Uniform<uint32_t>, Uniform<float>>& Shape<Prim>::GetColor()
{
  return _shape_color;
}
template <GLenum Prim>
Uniform<glm::vec3, float>& Shape<Prim>::GetLocation()
{
  return _location;
}

template <GLenum Prim>
Uniform<glm::vec3, float>& Shape<Prim>::GetScale()
{
  return _scale;
}

template <GLenum Prim>
Uniform<glm::vec3, float>& Shape<Prim>::GetRotate()
{
  return _rotation;
}

template <GLenum Prim>
Uniform<glm::mat4, float> Shape<Prim>::Model() const
{
  auto I = glm::identity<glm::mat4>();
  auto rotation = _rotation.GetValue();
  auto scale = _scale.GetValue() * _shapeRadius.GetValue();
  auto location = _location.GetValue();
  auto angle = glm::max(glm::max(rotation.x, rotation.y), rotation.z);
  auto T =
      glm::dot(location, location) != 0.f ? glm::translate(I, location) : I;
  auto R = glm::dot(rotation, rotation) != 0.f
               ? glm::rotate(I, glm::radians(angle), glm::normalize(rotation))
               : I;
  auto S = glm::dot(scale, scale) != 0.f ? glm::scale(I, scale) : I;
  return Uniform<glm::mat4, float>{T * R * S, "model"};
}

template <GLenum Prim>
GLenum Shape<Prim>::GetDrawPrimitive() const
{
  return _primitiveType;
}

template <GLenum Prim>
VertexArray const& Shape<Prim>::GetVA() const
{
  return _vertexArray;
}

template <GLenum Prim>
Uniform<glm::vec3, float> const& Shape<Prim>::GetCenter() const
{
  return _center;
}

template <GLenum Prim>
bool& Shape<Prim>::EnableTesselation()
{
  return _enableTesselation;
}
template <GLenum Prim>
void Shape<Prim>::EnableLight(bool light)
{
  _enableLight = light;
}

template <GLenum Prim>
Uniform<uint32_t>& Shape<Prim>::GetSubdivision()
{
  return _subdivision;
}
template <GLenum Prim>
Uniform<float>& Shape<Prim>::GetRadius()
{
  return _shapeRadius;
}

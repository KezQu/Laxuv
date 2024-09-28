#pragma once

#include <Camera.h>
#include <ProgramDispatch.h>
#include <VertexArray.h>

#include "Essentials.h"
#include "Uniform.h"

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
 protected:
  VertexArray _vertexArray;
  glm::vec3 _translation{0.f};
  glm::vec3 _scale{1.f};
  glm::vec3 _rotation{0.f};
  Uniform<glm::vec3> _center{0.f, "center "};
  Uniform<uint32_t> _subdivision{1U, "subdivision"};
  Uniform<uint32_t> _shapeRadius{0U, "shapeRadius"};
  bool _enableTesselation{false};
  bool _enableLight{true};
  GLenum _primitiveType{Prim};

 public:
  Shape(VertexArray&& vertexArray = {}, uint32_t shapeRadius = 100,
        bool enableTess = false);
  Shape(Shape const& obj_copy) = delete;
  Shape(Shape&& obj_move) = default;
  Shape& operator=(Shape const& obj_copy) = delete;
  Shape& operator=(Shape&& obj_move) = default;
  virtual ~Shape() = default;
  glm::vec3& GetLocation();
  glm::vec3& GetScale();
  glm::vec3& GetRotate();
  Uniform<glm::mat4> Model() const;
  void Bind() const;
  GLenum GetDrawPrimitive() const;
  VertexArray const& GetVA() const;
  glm::vec3 const& GetCenter() const;
  bool& EnableLight();
  bool& EnableTesselation();
  uint32_t& GetSubdivision();
  uint32_t& GetRadius();
  virtual Essentials::DistributionShape GetParticleDistribution() = 0;
};

template <GLenum Prim>
Shape<Prim>::Shape(VertexArray&& vertexArray, uint32_t shapeRadius,
                   bool enableTess)
    : _vertexArray{std::move(vertexArray)},
      _shapeRadius{shapeRadius},
      _enableTesselation{enableTess}
{
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
  _center /= coordBuffer.Size() / coordBuffer.AttribSize();
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

  Model().MapUniform(renderer.ID());
  View().MapUniform(renderer.ID());
  Projection().MapUniform(renderer.ID());
  Camera::GetCamera().Viewport().MapUniform(renderer.ID());
  _center.MapUniform(renderer.ID());
  _subdivision.MapUniform(renderer.ID());

  if (_enableLight)
  {
    auto const ambient_light =
        Uniform<glm::vec3>(LIGHT.AmbientLightColor, "ambientLightColor");
    auto const diffuse_light_color =
        Uniform<glm::vec3>(LIGHT.DiffuseLight.color, "diffuseLightColor");
    auto const diffuse_light_dir = Uniform<glm::vec3>(
        LIGHT.DiffuseLight.direction, "diffuseLightDirection");
    ambient_light.MapUniform(renderer.ID());
    diffuse_light_color.MapUniform(renderer.ID());
    diffuse_light_dir.MapUniform(renderer.ID());
  }
  _shapeRadius.MapUniform(renderer.ID());
  Uniform<uint32_t> const _kernel_radius{4U, "KernelRadius"};
  _kernel_radius.MapUniform(renderer.ID());
}

template <GLenum Prim>
glm::vec3& Shape<Prim>::GetLocation()
{
  return _translation;
}

template <GLenum Prim>
glm::vec3& Shape<Prim>::GetScale()
{
  return _scale;
}

template <GLenum Prim>
glm::vec3& Shape<Prim>::GetRotate()
{
  return _rotation;
}

template <GLenum Prim>
Uniform<glm::mat4> Shape<Prim>::Model() const
{
  auto I = glm::identity<glm::mat4>();
  auto angle = glm::max(glm::max(_rotation.x, _rotation.y), _rotation.z);
  auto T = glm::dot(_translation, _translation) != 0.f
               ? glm::translate(I, _translation)
               : I;
  auto R = glm::dot(_rotation, _rotation) != 0.f
               ? glm::rotate(I, glm::radians(angle), glm::normalize(_rotation))
               : I;
  auto S = glm::dot(_scale, _scale) != 0.f ? glm::scale(I, _scale) : I;
  return Unifrom<glm::mat4>{T * R * S, "model"};
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
glm::vec3 const& Shape<Prim>::GetCenter() const
{
  return _center;
}

template <GLenum Prim>
bool& Shape<Prim>::EnableLight()
{
  return _enableLight;
}
template <GLenum Prim>
bool& Shape<Prim>::EnableTesselation()
{
  return _enableTesselation;
}
template <GLenum Prim>
uint32_t& Shape<Prim>::GetSubdivision()
{
  return _subdivision;
}
template <GLenum Prim>
uint32_t& Shape<Prim>::GetRadius()
{
  return _shapeRadius;
}

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
 protected:
  VertexArray _vertexArray;
  Essentials::ShapeProperties _shape_properties;
  GLenum _primitiveType{Prim};

 public:
  Shape(VertexArray&& vertexArray = {}, float shapeRadius = 10.f,
        bool enableTess = false);
  Shape(Shape const& obj_copy) = delete;
  Shape(Shape&& obj_move) = default;
  Shape& operator=(Shape const& obj_copy) = delete;
  Shape& operator=(Shape&& obj_move) = default;
  virtual ~Shape() = default;
  Essentials::ShapeProperties& GetShapeProperties();
  Uniform<glm::mat4, float> Model() const;
  void Bind() const;
  GLenum GetDrawPrimitive() const;
  VertexArray const& GetVA() const;
  void SetTesselation(bool enableTesselation);
  bool GetTesselation() const;
  void EnableLight(bool light);
};

template <GLenum Prim>
Shape<Prim>::Shape(VertexArray&& vertexArray, float shapeRadius,
                   bool enableTess)
    : _vertexArray{std::move(vertexArray)}
{
  _shape_properties._enableTesselation = enableTess;
  _shape_properties._radius = shapeRadius;
  if (_shape_properties._enableTesselation == true)
  {
    _primitiveType = GL_PATCHES;
  }
  auto& coordBuffer = GetVA().Data().coordinateBuffer;

  for (int i = 0; i < coordBuffer.Size(); i += 3)
  {
    _shape_properties._center +=
        glm::vec3(coordBuffer.Data()[i + 0], coordBuffer.Data()[i + 1],
                  coordBuffer.Data()[i + 2]);
  }
  _shape_properties._center /=
      static_cast<float>(coordBuffer.Size() / coordBuffer.AttribSize());
}

template <GLenum Prim>
void Shape<Prim>::Bind() const
{
  _vertexArray.Bind();

  Program& renderer =
      _shape_properties._enableTesselation == true
          ? ProgramDispatch::GetInstance().GetTesselationPipeline()
          : ProgramDispatch::GetInstance().GetSimplePipeline();

  if (!renderer.isLinked()) renderer.Link();
  renderer.Bind();

  Simulator::GetInstance().BindUniforms(renderer.ID());

  Camera::GetCamera().View().MapUniform(renderer.ID());
  Camera::GetCamera().Projection().MapUniform(renderer.ID());
  Camera::GetCamera().Viewport().MapUniform(renderer.ID());
  _shape_properties.Model().MapUniform(renderer.ID());
  _shape_properties._center.MapUniform(renderer.ID());
  _shape_properties._subdivision.MapUniform(renderer.ID());

  _shape_properties._color.first.MapUniform(renderer.ID());
  _shape_properties._color.second.MapUniform(renderer.ID());
  if (_shape_properties._enableLight)
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
  _shape_properties._radius.MapUniform(renderer.ID());
}

template <GLenum Prim>
Essentials::ShapeProperties& Shape<Prim>::GetShapeProperties()
{
  return _shape_properties;
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
void Shape<Prim>::SetTesselation(bool enableTesselation)
{
  _shape_properties._enableTesselation = enableTesselation;
}

template <GLenum Prim>
bool Shape<Prim>::GetTesselation() const
{
  return _shape_properties._enableTesselation;
}

template <GLenum Prim>
void Shape<Prim>::EnableLight(bool light)
{
  _shape_properties._enableLight = light;
}

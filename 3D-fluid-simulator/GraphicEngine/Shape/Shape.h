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
  void Bind(uint32_t program_id) const;
  void BindUniforms(uint32_t program_id) const;
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
void Shape<Prim>::Bind(uint32_t program_id) const
{
  _vertexArray.Bind();
  BindUniforms(program_id);
}

template <GLenum Prim>
void Shape<Prim>::BindUniforms(uint32_t program_id) const
{
  Camera::GetCamera().View().MapUniform(program_id);
  Camera::GetCamera().Projection().MapUniform(program_id);
  Camera::GetCamera().Viewport().MapUniform(program_id);
  _shape_properties.Model().MapUniform(program_id);
  _shape_properties._scale.MapUniform(program_id);
  _shape_properties._center.MapUniform(program_id);
  _shape_properties._subdivision.MapUniform(program_id);

  _shape_properties._color.first.MapUniform(program_id);
  _shape_properties._color.second.MapUniform(program_id);
  if (_shape_properties._enableLight)
  {
    auto const ambient_light =
        Uniform<glm::vec3, float>(LIGHT.AmbientLightColor, "ambientLightColor");
    auto const diffuse_light_color = Uniform<glm::vec3, float>(
        LIGHT.DiffuseLight.color, "diffuseLightColor");
    auto const diffuse_light_dir = Uniform<glm::vec3, float>(
        LIGHT.DiffuseLight.direction, "diffuseLightDirection");
    ambient_light.MapUniform(program_id);
    diffuse_light_color.MapUniform(program_id);
    diffuse_light_dir.MapUniform(program_id);
  }
  _shape_properties._radius.MapUniform(program_id);
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

#pragma once

#include "Camera.h"
#include "Essentials.h"
#include "ProgramDispatch.h"
#include "Simulator.h"
#include "Uniform.h"
#include "VertexArray.h"

/**
 * @brief Base class providing interface for creation an abstract called shape
 * which define the model displayed in the simulation context
 *
 */
template <GLenum Prim>
class Shape
{
 protected:
  VertexArray _vertexArray;
  Essentials::ShapeProperties _shape_properties;
  GLenum _primitiveType{Prim};

 public:
  /**
   * @brief Create a shape for a given vertex array specifying vertices defining
   * a shape
   *
   * @param vertexArray VAO specifying vertices defining shape
   * @param shapeRadius Initial scaling factor of the vertex position
   * @param enableTess Flag defining whether tesselation is needed to create
   * shape
   */
  Shape(VertexArray&& vertexArray = {}, float shapeRadius = 10.f,
        bool enableTess = false);
  Shape(Shape const& obj_copy) = delete;
  Shape(Shape&& obj_move) = default;
  Shape& operator=(Shape const& obj_copy) = delete;
  Shape& operator=(Shape&& obj_move) = default;
  virtual ~Shape() = default;
  /**
   * @brief Getter providing access to the properties of the shape
   *
   * @return Essentials::ShapeProperties&
   */
  Essentials::ShapeProperties& GetShapeProperties();
  /**
   * @brief Bind stored VAO and uniforms to a given program
   *
   * @param program_id id program to bind the shape properties to
   */
  void Bind(uint32_t program_id) const;
  /**
   * @brief Helper method responsible for binding uniforms specific to the shape
   *
   * @param program_id id program to bind the shape properties to
   */
  void BindUniforms(uint32_t program_id) const;
  /**
   * @brief Getter allowing ot retrieve primitive used to generate shape using
   * OpenGL
   *
   * @return GLenum
   */
  GLenum GetDrawPrimitive() const;
  /**
   * @brief Getter providing read-only access to the VAO used for the shape
   *
   * @return VertexArray const&
   */
  VertexArray const& GetVA() const;
  /**
   * @brief Getter allowing to retrieve flag defining whether tesselation is
   * used to generate current shape
   *
   * @return true
   * @return false
   */
  bool GetTesselation() const;
  /**
   * @brief Helper method used to define id the shape should react to the
   * lightning
   *
   * @param light Flag indicating if the lightning should be taken into account
   */
  void EnableLight(bool light);
};

template <GLenum Prim>
Shape<Prim>::Shape(VertexArray&& vertexArray, float shapeRadius,
                   bool enableTess)
    : _vertexArray{std::move(vertexArray)}
{
  // Save specified data to the shape memory
  _shape_properties._enableTesselation = enableTess;
  _shape_properties._radius = shapeRadius;
  // Change the primitive type if the tesselation is enabled
  if (_shape_properties._enableTesselation == true)
  {
    _primitiveType = GL_PATCHES;
  }

  // Recreate center on the shape based on the arithmetic mean of the vertices
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
  // Bind corresponding VAO and needed uniforms
  _vertexArray.Bind();
  BindUniforms(program_id);
}

template <GLenum Prim>
void Shape<Prim>::BindUniforms(uint32_t program_id) const
{
  // Map transformation matrices to transform shape into the world coordinates
  // inside the rendering pipeline
  Camera::GetCamera().View().MapUniform(program_id);
  Camera::GetCamera().Projection().MapUniform(program_id);
  Camera::GetCamera().Viewport().MapUniform(program_id);
  // Map shape properties for further transformation in the rendering pipeline
  _shape_properties.Model().MapUniform(program_id);
  _shape_properties._scale.MapUniform(program_id);
  _shape_properties._center.MapUniform(program_id);
  _shape_properties._subdivision.MapUniform(program_id);

  _shape_properties._color.first.MapUniform(program_id);
  _shape_properties._color.second.MapUniform(program_id);
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
bool Shape<Prim>::GetTesselation() const
{
  return _shape_properties._enableTesselation;
}

template <GLenum Prim>
void Shape<Prim>::EnableLight(bool light)
{
  _shape_properties._enableLight = light;
}

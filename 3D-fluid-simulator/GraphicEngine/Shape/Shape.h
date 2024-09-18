#pragma once

#include <ProgramDispatch.h>
#include <VertexArray.h>
#include <Camera.h>
#include "Essentials.h"

struct {
	glm::vec3 AmbientLightColor{ .2f };
	struct {
		glm::vec3 color{ 1.f };
		glm::vec3 direction{ -0.3f, -0.2f, -1.f };
	} DiffuseLight;
}LIGHT;

template<GLenum Prim>
class Shape {
protected:
	VertexArray _vertexArray;
	glm::vec3 _translation{ 0.f };
	glm::vec3 _scale{ 1.f };
	glm::vec3 _rotation{ 0.f };
	glm::vec3 _center{ 0.f };
	uint32_t _subdivision{ 1 };
	uint32_t _shapeRadius;
	bool _enableLight{ true };
	bool _enableTesselation{ false };
	GLenum _primitiveType{ Prim };
public:
	Shape(VertexArray&& vertexArray = {}, uint32_t shapeRadius = 100, bool enableTess = false);
	Shape(Shape const& obj_copy) = delete;
	Shape(Shape && obj_move) = default;
	Shape& operator=(Shape const& obj_copy) = delete;
	Shape& operator=(Shape && obj_move) = default;
	virtual ~Shape() = default;
	glm::vec3& GetLocation();
	glm::vec3& GetScale();
	glm::vec3& GetRotate();
	glm::mat4 Model() const;
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

template<GLenum Prim>
Shape<Prim>::Shape(VertexArray&& vertexArray, uint32_t shapeRadius, bool enableTess)
	:_vertexArray{ std::move(vertexArray) },
	_shapeRadius{ shapeRadius },
	_enableTesselation{ enableTess }
{
	if (_enableTesselation == true) {
		_primitiveType = GL_PATCHES;
	}
	auto& coordBuffer = GetVA().Data().coordinateBuffer;

	for (int i = 0; i < coordBuffer.Size(); i += 3) {
		_center += glm::vec3(coordBuffer.Data()[i + 0],
							 coordBuffer.Data()[i + 1],
							 coordBuffer.Data()[i + 2]);
	}
	_center /= coordBuffer.Size() / coordBuffer.AttribSize();
}

template<GLenum Prim>
void Shape<Prim>::Bind() const {
	_vertexArray.Bind();
	
	Program& renderer = _enableTesselation == true ?
		ProgramDispatch::GetInstance().GetTesselationPipeline():
		ProgramDispatch::GetInstance().GetSimplePipeline();
	
	if (!renderer.isLinked())
		renderer.Link();
	renderer.Bind();
	
	GLint modelLocation = glGetProgramResourceLocation(renderer.ID(), GL_UNIFORM, "model");
	GLint viewLocation = glGetProgramResourceLocation(renderer.ID(), GL_UNIFORM, "view");
	GLint projectionLocation = glGetProgramResourceLocation(renderer.ID(), GL_UNIFORM, "projection");
	GLint viewportLocation = glGetProgramResourceLocation(renderer.ID(), GL_UNIFORM, "viewport");
	GLint centerLocation = glGetProgramResourceLocation(renderer.ID(), GL_UNIFORM, "center");
	GLint subdivisionLocation = glGetProgramResourceLocation(renderer.ID(), GL_UNIFORM, "subdivision");
	GLint ambientLightColorLocation = glGetProgramResourceLocation(renderer.ID(), GL_UNIFORM, "ambientLightColor");
	GLint diffuseLightColorLocation = glGetProgramResourceLocation(renderer.ID(), GL_UNIFORM, "diffuseLightColor");
	GLint diffuseLightDirectionLocation = glGetProgramResourceLocation(renderer.ID(), GL_UNIFORM, "diffuseLightDirection");
	GLint shapeRadiusLocation = glGetProgramResourceLocation(renderer.ID(), GL_UNIFORM, "shapeRadius");
	GLint kernelRadiusLoc = glGetProgramResourceLocation(renderer.ID(), GL_UNIFORM, "KernelRadius");

	if (modelLocation != -1)
		glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(Model()));
	if (viewLocation != -1)
		glUniformMatrix4fv(viewLocation, 1, GL_FALSE, glm::value_ptr(Camera::GetCamera().View()));
	if (projectionLocation != -1)
		glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, glm::value_ptr(Camera::GetCamera().Projection()));
	if (viewportLocation != -1)
		glUniform2fv(viewportLocation, 1, glm::value_ptr(Camera::GetCamera().Viewport()));
		
	if (centerLocation != -1)
		glUniform3fv(centerLocation, 1, glm::value_ptr(_center));
	if (subdivisionLocation != -1)
		glUniform1i(subdivisionLocation, _subdivision);
	if (ambientLightColorLocation != -1) {
		auto objAmbientLightColor = _enableLight ? LIGHT.AmbientLightColor : glm::vec3(1.0);
		glUniform3fv(ambientLightColorLocation, 1, glm::value_ptr(objAmbientLightColor));
	}
	if (diffuseLightColorLocation != -1) {
		auto objDiffuseLightColor = _enableLight ? LIGHT.DiffuseLight.color : glm::vec3(1.0);
		glUniform3fv(diffuseLightColorLocation, 1, glm::value_ptr(objDiffuseLightColor));
	}
	if (diffuseLightDirectionLocation != -1) {
		auto objDiffuseLightDirection = _enableLight ? LIGHT.DiffuseLight.direction : glm::vec3(0.0);
		glUniform3fv(diffuseLightDirectionLocation, 1, glm::value_ptr(objDiffuseLightDirection));
	}
	if (shapeRadiusLocation != -1) {
		glUniform1ui(shapeRadiusLocation, _shapeRadius);
	}
	if (kernelRadiusLoc != -1) {
		glUniform1ui(kernelRadiusLoc, static_cast<uint32_t>(4));
	}
}

template<GLenum Prim>
glm::vec3& Shape<Prim>::GetLocation() {
	return _translation;
}

template<GLenum Prim>
glm::vec3& Shape<Prim>::GetScale() {
	return _scale;
}

template<GLenum Prim>
glm::vec3& Shape<Prim>::GetRotate() {
	return _rotation;
}

template<GLenum Prim>
glm::mat4 Shape<Prim>::Model() const {
	auto I = glm::identity<glm::mat4>();
	auto angle = glm::max(glm::max(_rotation.x, _rotation.y), _rotation.z);
	auto T = glm::dot(_translation, _translation) != 0.f ? glm::translate(I, _translation) : I;
	auto R = glm::dot(_rotation, _rotation) != 0.f ? glm::rotate(I, glm::radians(angle), glm::normalize(_rotation)) : I;
	auto S = glm::dot(_scale, _scale) != 0.f ? glm::scale(I, _scale) : I;
	return T * R * S;
}

template<GLenum Prim>
GLenum Shape<Prim>::GetDrawPrimitive() const {
	return _primitiveType;
}

template<GLenum Prim>
VertexArray const& Shape<Prim>::GetVA() const {
	return _vertexArray;
}

template<GLenum Prim>
glm::vec3 const& Shape<Prim>::GetCenter() const {
	return _center;
}

template<GLenum Prim>
bool& Shape<Prim>::EnableLight() {
	return _enableLight;
}
template<GLenum Prim>
bool& Shape<Prim>::EnableTesselation() {
	return _enableTesselation;
}
template<GLenum Prim>
uint32_t& Shape<Prim>::GetSubdivision() {
	return _subdivision;
}
template<GLenum Prim>
uint32_t& Shape<Prim>::GetRadius() {
	return _shapeRadius;
}

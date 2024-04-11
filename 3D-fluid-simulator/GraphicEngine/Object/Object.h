#pragma once

#include <Program.h>
#include <VertexArray.h>
#include <Camera.h>

struct {
	glm::vec3 AmbientLightColor{ .2f };
	struct {
		glm::vec3 color{ 1.f }; 
		glm::vec3 direction{ -0.3f, -0.2f, -1.f };
	} DiffuseLight;
}LIGHT;

template<GLenum Prim>
class Object{
protected:
	VertexArray _vertexArray;
	Program _renderer;
	glm::vec3 _translation{ 0.f }, _scale{ 1.f }, _rotation{ 0.f }, _normal{ 0.f }, _center{ 0.f };
	GLenum _primitiveType = Prim;
	int _subdivision{ 1 };
	bool _enableLight{ true };
	bool _enableTesselation;
protected:
	Object(VertexArray&& vertexArray = {}, bool enableTess = false);
public:
	void Draw() const;
	void BindUniforms() const;
	Object& Move(glm::vec3 translation);
	Object& Scale(glm::vec3 scale);
	Object& Rotate(glm::vec3 rotation);
	glm::mat4 Model() const;
	void EnableLight(bool enable);
};

template<GLenum Prim>
Object<Prim>::Object(VertexArray&& vertexArray, bool enableTess)
	:_vertexArray{ std::move(vertexArray) },
	_enableTesselation{ enableTess }
{ 
	_renderer.AddShader(GL_VERTEX_SHADER, "/Object.vert");
	if (_enableTesselation) {
		_primitiveType = GL_PATCHES;

		_renderer.AddShader(GL_TESS_CONTROL_SHADER, "/Object.tesc");
		_renderer.AddShader(GL_TESS_EVALUATION_SHADER, "/Object.tese");
	}
	if (_primitiveType == GL_LINES) {
		_renderer.AddShader(GL_GEOMETRY_SHADER, "/ObjectLines.geom");
	}
	else {
		_renderer.AddShader(GL_GEOMETRY_SHADER, "/ObjectTriangles.geom");
	}
	_renderer.AddShader(GL_GEOMETRY_SHADER, "/CalculateNDC.glsl");
	_renderer.AddShader(GL_GEOMETRY_SHADER, "/CalculateNormal.glsl");
	_renderer.AddShader(GL_FRAGMENT_SHADER, "/Object.frag");
}

template<GLenum Prim>
void Object<Prim>::Draw() const
{
	_vertexArray.Bind();
	if (!_renderer.isLinked())
		_renderer.Link();
	_renderer.Bind();

	BindUniforms();
	glDrawElements(_primitiveType, _vertexArray.Size(), _vertexArray.IndexBufferType(), nullptr);
}

template<GLenum Prim>
void Object<Prim>::BindUniforms() const {
	GLint modelLocation = glGetProgramResourceLocation(_renderer.ID(), GL_UNIFORM, "model");
	GLint viewLocation = glGetProgramResourceLocation(_renderer.ID(), GL_UNIFORM, "view");
	GLint projectionLocation = glGetProgramResourceLocation(_renderer.ID(), GL_UNIFORM, "projection");
	GLint centerLocation = glGetProgramResourceLocation(_renderer.ID(), GL_UNIFORM, "center");
	GLint subdivisionLocation = glGetProgramResourceLocation(_renderer.ID(), GL_UNIFORM, "subdivision");
	GLint ambientLightColorLocation = glGetProgramResourceLocation(_renderer.ID(), GL_UNIFORM, "ambientLightColor");
	GLint diffuseLightColorLocation = glGetProgramResourceLocation(_renderer.ID(), GL_UNIFORM, "diffuseLightColor");
	GLint diffuseLightDirectionLocation = glGetProgramResourceLocation(_renderer.ID(), GL_UNIFORM, "diffuseLightDirection");

	if (modelLocation != -1)
		glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(Model()));
	if (viewLocation != -1)
		glUniformMatrix4fv(viewLocation, 1, GL_FALSE, glm::value_ptr(Camera::GetCamera().View()));
	if (projectionLocation != -1)
		glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, glm::value_ptr(Camera::GetCamera().Projection()));
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
		auto objDiffuseLightDirection = _enableLight ? LIGHT.DiffuseLight.direction: glm::vec3(0.0);
		glUniform3fv(diffuseLightDirectionLocation, 1, glm::value_ptr(objDiffuseLightDirection));
	}
}

template<GLenum Prim>
Object<Prim>& Object<Prim>::Move(glm::vec3 translation) {
	_translation = translation;
	return *this;
}

template<GLenum Prim>
Object<Prim>& Object<Prim>::Scale(glm::vec3 scale) {
	_scale = scale;
	return *this;
}

template<GLenum Prim>
Object<Prim>& Object<Prim>::Rotate(glm::vec3 rotation) {
	_rotation = rotation;
	return *this;
}

template<GLenum Prim>
glm::mat4 Object<Prim>::Model() const {
	auto I = glm::identity<glm::mat4>();
	auto angle = glm::max(glm::max(_rotation.x, _rotation.y), _rotation.z);
	auto T = glm::dot(_translation, _translation) != 0.f ? glm::translate(I, _translation) : I;
	auto R = glm::dot(_rotation, _rotation) != 0.f ? glm::rotate(I, glm::radians(angle), glm::normalize(_rotation)) : I;
	auto S = glm::dot(_scale, _scale) != 0.f ? glm::scale(I, _scale) : I;
	return T * R * S;
}

template<GLenum Prim>
void Object<Prim>::EnableLight(bool enable) {
	_enableLight = enable;
}
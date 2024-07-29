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
class Shape {
protected:
	VertexArray _vertexArray;
	Program _renderer;
	glm::vec3 _translation{ 0.f };
	glm::vec3 _scale{ 1.f };
	glm::vec3 _rotation{ 0.f };
	glm::vec3 _center{ 0.f };
	int _subdivision{ 1 };
	int _shapeRadius;
	bool _enableLight{ true };
	bool _enableTesselation{ false };
public:
	inline static GLenum _primitiveType{ Prim };
public:
	Shape(VertexArray&& vertexArray = {}, int shapeRadius = 100, bool enableTess = false);
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
	Program const& GetRenderer() const;
	Program& GetRenderer();
	glm::vec3 const& GetCenter() const;
	bool& EnableLight();
	bool& EnableTesselation();
	int& ChangeSubdivision();
	int& ChangeRadius();
};

template<GLenum Prim>
Shape<Prim>::Shape(VertexArray&& vertexArray, int shapeRadius, bool enableTess)
	:_vertexArray{ std::move(vertexArray) },
	_shapeRadius{ shapeRadius },
	_enableTesselation{ enableTess }
{
	auto& coordBuffer = GetVA().Data().coordinateBuffer;
	for (int i = 0; i < coordBuffer.Size(); i += 3) {
		_center += glm::vec3(coordBuffer.Data()[i + 0],
							 coordBuffer.Data()[i + 1],
							 coordBuffer.Data()[i + 2]);
	}
	_center /= coordBuffer.Size() / coordBuffer.AttribSize();

	_renderer.AddShader(GL_VERTEX_SHADER, "/Element.vert");
	//_renderer.AddShader(GL_VERTEX_SHADER, "/CalculateOffset.glsl");
	if (_enableTesselation) {
		_primitiveType = GL_PATCHES;

		_renderer.AddShader(GL_TESS_CONTROL_SHADER, "/Element.tesc");
		_renderer.AddShader(GL_TESS_EVALUATION_SHADER, "/Element.tese");
	}
	if (_primitiveType == GL_LINES) {
		_renderer.AddShader(GL_GEOMETRY_SHADER, "/Lines.geom");
	}
	else if (_primitiveType == GL_POINTS) {
		_renderer.AddShader(GL_GEOMETRY_SHADER, "/Points.geom");
	}
	else {
		_renderer.AddShader(GL_GEOMETRY_SHADER, "/Triangles.geom");
	}
	_renderer.AddShader(GL_GEOMETRY_SHADER, "/CalculateNDC.glsl");
	_renderer.AddShader(GL_GEOMETRY_SHADER, "/CalculateNormal.glsl");
	_renderer.AddShader(GL_FRAGMENT_SHADER, "/Element.frag");
}

template<GLenum Prim>
void Shape<Prim>::Bind() const {
	_vertexArray.Bind();
	if (!_renderer.isLinked())
		_renderer.Link();
	_renderer.Bind();
	
	GLint modelLocation = glGetProgramResourceLocation(_renderer.ID(), GL_UNIFORM, "model");
	GLint viewLocation = glGetProgramResourceLocation(_renderer.ID(), GL_UNIFORM, "view");
	GLint projectionLocation = glGetProgramResourceLocation(_renderer.ID(), GL_UNIFORM, "projection");
	GLint viewportLocation = glGetProgramResourceLocation(_renderer.ID(), GL_UNIFORM, "viewport");
	GLint centerLocation = glGetProgramResourceLocation(_renderer.ID(), GL_UNIFORM, "center");
	GLint subdivisionLocation = glGetProgramResourceLocation(_renderer.ID(), GL_UNIFORM, "subdivision");
	GLint ambientLightColorLocation = glGetProgramResourceLocation(_renderer.ID(), GL_UNIFORM, "ambientLightColor");
	GLint diffuseLightColorLocation = glGetProgramResourceLocation(_renderer.ID(), GL_UNIFORM, "diffuseLightColor");
	GLint diffuseLightDirectionLocation = glGetProgramResourceLocation(_renderer.ID(), GL_UNIFORM, "diffuseLightDirection");
	GLint shapeRadiusLocation = glGetProgramResourceLocation(_renderer.ID(), GL_UNIFORM, "shapeRadius");

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
		glUniform1i(shapeRadiusLocation, _shapeRadius);
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
Program const& Shape<Prim>::GetRenderer() const {
	return _renderer;

}
template<GLenum Prim>
Program& Shape<Prim>::GetRenderer() {
	return _renderer;
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
int& Shape<Prim>::ChangeSubdivision() {
	return _subdivision;
}
template<GLenum Prim>
int& Shape<Prim>::ChangeRadius() {
	return _shapeRadius;
}

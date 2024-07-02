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
class Element {
protected:
	VertexArray _vertexArray;
	Program _renderer;
	glm::vec3 _translation{ 0.f }, _scale{ 1.f }, _rotation{ 0.f }, _center{ 0.f }, _boundingBox{ 0.f };
	GLenum _primitiveType{ Prim };
	int _subdivision{ 1 };
	bool _enableLight{ true }, _enableTesselation{ false };
protected:
	Element(VertexArray&& vertexArray = {}, bool enableTess = false);
	Element(Element const& obj_copy) = delete;
	Element(Element && obj_move) = default;
	Element& operator=(Element const& obj_copy) = delete;
	Element& operator=(Element && obj_move) = default;
	virtual ~Element();
public:
	Element<Prim>& Move(glm::vec3 translation);
	Element<Prim>& Scale(glm::vec3 scale);
	Element<Prim>& Rotate(glm::vec3 rotation);
	glm::mat4 Model() const;
	void Bind() const;
	GLenum GetDrawPrimitive() const;
	VertexArray const& GetVA() const;
	Program const& GetRenderer() const;
	glm::vec3 const& GetCenter() const;
	glm::vec3 const& GetBoundingBox() const;
	Element<Prim>& EnableLight(bool enable);
	Element<Prim>& ChangeSubdivision(int newSubdivision);
};

template<GLenum Prim>
Element<Prim>::Element(VertexArray&& vertexArray, bool enableTess)
	:_vertexArray{ std::move(vertexArray) },
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
	_renderer.AddShader(GL_VERTEX_SHADER, "/CalculateOffset.glsl");
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
Element<Prim>::~Element() {}

template<GLenum Prim>
void Element<Prim>::Bind() const {
	_vertexArray.Bind();
	if (!_renderer.isLinked())
		_renderer.Link();
	_renderer.Bind();
 
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
		auto objDiffuseLightDirection = _enableLight ? LIGHT.DiffuseLight.direction : glm::vec3(0.0);
		glUniform3fv(diffuseLightDirectionLocation, 1, glm::value_ptr(objDiffuseLightDirection));
	}
}

template<GLenum Prim>
Element<Prim>& Element<Prim>::Move(glm::vec3 translation) {
	_translation = translation;
	return *this;
}

template<GLenum Prim>
Element<Prim>& Element<Prim>::Scale(glm::vec3 scale) {
	_scale = scale;
	return *this;
}

template<GLenum Prim>
Element<Prim>& Element<Prim>::Rotate(glm::vec3 rotation) {
	_rotation = rotation;
	return *this;
}

template<GLenum Prim>
glm::mat4 Element<Prim>::Model() const {
	auto I = glm::identity<glm::mat4>();
	auto angle = glm::max(glm::max(_rotation.x, _rotation.y), _rotation.z);
	auto T = glm::dot(_translation, _translation) != 0.f ? glm::translate(I, _translation) : I;
	auto R = glm::dot(_rotation, _rotation) != 0.f ? glm::rotate(I, glm::radians(angle), glm::normalize(_rotation)) : I;
	auto S = glm::dot(_scale, _scale) != 0.f ? glm::scale(I, _scale) : I;
	return T * R * S;
}

template<GLenum Prim>
GLenum Element<Prim>::GetDrawPrimitive() const {
	return _primitiveType;
}
template<GLenum Prim>
VertexArray const& Element<Prim>::GetVA() const {
	return _vertexArray;
}
template<GLenum Prim>
Program const& Element<Prim>::GetRenderer() const {
	return _renderer;

}
template<GLenum Prim>
glm::vec3 const& Element<Prim>::GetCenter() const {
	return _center;
}
template<GLenum Prim>
glm::vec3 const& Element<Prim>::GetBoundingBox() const {
	return _boundingBox;
}

template<GLenum Prim>
Element<Prim>& Element<Prim>::EnableLight(bool enable) {
	_enableLight = enable;
	return *this;
}
template<GLenum Prim>
Element<Prim>& Element<Prim>::ChangeSubdivision(int newSubdivision) {
	_subdivision = newSubdivision;
	return *this;
}

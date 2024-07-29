#pragma once

#include <functional>

#include <Shape.h>
#include <Entity.h>

template<typename T>
class Particles : public Entity {
private:
	enum DistributionType{
		PLANE = 0,
		LINE = 1,
		QUBE = 2,
		SPHERE = 3,
		DISK = 4
	};
	T _particleShape;
	std::size_t _count;
	int _distribution{ DistributionType::QUBE };
public:
	Particles(T&& particleShape, std::size_t count);
	Particles(Particles<T> const& obj_copy) = delete;
	Particles(Particles<T>&& obj_move) = default;
	Particles& operator=(Particles<T> const& obj_copy) = delete;
	Particles& operator=(Particles<T>&& obj_move) = default;
	~Particles() override = default;
	void Draw() const override;
	void Bind() const;
	details_map Details() override;
	int& Distribution();
	 std::size_t& Count();
};

template<typename T>
Particles<T>::Particles(T&& particleShape, std::size_t count)
	:_particleShape(std::exchange(particleShape, T())),
	_count{ count }
{
	//if (_particleShape.GetDrawPrimitive() == GL_LINES) {
	//	_particleShape.GetRenderer().AddShader(GL_GEOMETRY_SHADER, "/Lines.geom");
	//}
	//else if (_particleShape.GetDrawPrimitive() == GL_POINTS) {
	//	_particleShape.GetRenderer().AddShader(GL_GEOMETRY_SHADER, "/Points.geom");
	//}
	//else {
	//	_particleShape.GetRenderer().AddShader(GL_GEOMETRY_SHADER, "/Triangles.geom");
	//}
}

template<typename T>
void Particles<T>::Draw() const
{
	if (_visible) {
		_particleShape.Bind();
		Bind();
		_(glDrawElementsInstanced(_particleShape.GetDrawPrimitive(), _particleShape.GetVA().Size(), _particleShape.GetVA().IndexBufferType(), nullptr, _count));
	}
}

template<typename T>
void Particles<T>::Bind() const
{}

template<typename T>
Particles<T>::details_map Particles<T>::Details()
{
	details_map details = Entity::Details();
	details.push_back({ "Location", { [=]() {return std::ref(this->_particleShape.GetLocation()); }, DetailsType::VEC3 } });
	details.push_back({ "Rotation", { [=]() {return std::ref(this->_particleShape.GetRotate()); }, DetailsType::VEC3 } });
	details.push_back({ "Scale", { [=]() {return std::ref(this->_particleShape.GetScale()); }, DetailsType::VEC3 } });
	details.push_back({ "Light", { [=]() {return std::ref(this->_particleShape.EnableLight()); }, DetailsType::BOOL } });
	details.push_back({ "Subdivision", { [=]() {return std::ref(this->_particleShape.ChangeSubdivision()); }, DetailsType::INT } });
	details.push_back({ "Radius", { [=]() {return std::ref(this->_particleShape.ChangeRadius()); }, DetailsType::INT } });
	details.push_back({ "Distribution", { [=]() {return std::ref(this->Distribution()); }, DetailsType::INT } });
	details.push_back({ "Count", { [=]() {return std::ref(this->Count()); }, DetailsType::SIZET } });
	return details;
}

template<typename T>
int& Particles<T>::Distribution()
{
	return _distribution;
}

template<typename T>
std::size_t& Particles<T>::Count()
{
	return _count;
}


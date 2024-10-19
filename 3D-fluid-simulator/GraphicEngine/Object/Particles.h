#pragma once

#include <Entity.h>
#include <Shape.h>

#include <functional>

template <GLenum Prim>
class Particles : public Entity
{
 private:
  std::unique_ptr<Shape<Prim>> _particleShape;

 public:
  Particles(Shape<Prim>* const particleShape, uint32_t meshRadius);
  Particles(Particles<Prim> const& obj_copy) = delete;
  Particles(Particles<Prim>&& obj_move) = default;
  Particles& operator=(Particles<Prim> const& obj_copy) = delete;
  Particles& operator=(Particles<Prim>&& obj_move) = default;
  ~Particles() override = default;
  void Initialize() override;
  void Calculate() override;
  void Draw() const override;
  void Bind() const;
  details::details_map Details() override;
};

template <GLenum Prim>
Particles<Prim>::Particles(Shape<Prim>* const particleShape,
                           uint32_t _meshRadius)
    : Entity{Essentials::PhysicsType::DYNAMIC},
      _particleShape{std::unique_ptr<Shape<Prim>>(particleShape)}
{
  Initialize();
}

template <GLenum Prim>
void Particles<Prim>::Initialize()
{
  _physicsDispatch.InitDefaultShape(GetPhysicsType(),
                                    _particleShape->GetRadius() * 2);
}

template <GLenum Prim>
void Particles<Prim>::Calculate()
{
  _physicsDispatch.GenerateForces(GetPhysicsType());
}

template <GLenum Prim>
void Particles<Prim>::Draw() const
{
  if (_visible)
  {
    _particleShape->Bind();
    uint32_t programID =
        _particleShape->EnableTesselation()
            ? ProgramDispatch::GetInstance().GetTesselationPipeline().ID()
            : ProgramDispatch::GetInstance().GetSimplePipeline().ID();

    _physicsDispatch.GetParticleMeshBuffer().Bind(programID);
    _(glDrawElementsInstanced(
        _particleShape->GetDrawPrimitive(), _particleShape->GetVA().Size(),
        _particleShape->GetVA().IndexBufferType(), nullptr,
        _physicsDispatch.GetParticleMeshBuffer().Size()));
    _physicsDispatch.GetParticleMeshBuffer().Unbind(programID);
  }
}

template <GLenum Prim>
details::details_map Particles<Prim>::Details()
{
  details::details_map details = Entity::Details();
  details.push_back(
      {"Location",
       {[this]() { return std::ref(this->_particleShape->GetLocation()); },
        details::DetailsType::VEC3}});
  details.push_back(
      {"Rotation",
       {[this]() { return std::ref(this->_particleShape->GetRotate()); },
        details::DetailsType::VEC3}});
  details.push_back(
      {"Scale",
       {[this]() { return std::ref(this->_particleShape->GetScale()); },
        details::DetailsType::VEC3}});
  details.push_back(
      {"Light",
       {[this]() { return std::ref(this->_particleShape->EnableLight()); },
        details::DetailsType::BOOL}});
  details.push_back(
      {"Subdivision",
       {[this]()
        { return std::ref(this->_particleShape->GetSubdivision().GetValue()); },
        details::DetailsType::UINT32}});
  details.push_back(
      {"Radius",
       {[this]()
        { return std::ref(this->_particleShape->GetRadius().GetValue()); },
        details::DetailsType::UINT32}});
  details.push_back({"Particle spacing",
                     {[this]()
                      {
                        return std::ref(
                            this->_physicsDispatch.GetFluidProperties()
                                .particle_spacing.GetValue());
                      },
                      details::DetailsType::FLOAT}});
  details.push_back({"Distribution",
                     {[this]()
                      {
                        return std::ref(
                            this->_physicsDispatch.GetFluidProperties()
                                .distribution_shape.GetValue());
                      },
                      details::DetailsType::UINT8}});
  details.push_back({"Physics",
                     {[this]() { return std::ref(this->GetPhysicsType()); },
                      details::DetailsType::PHYSTYPE}});
  details.push_back({"Influence kernel",
                     {[this]()
                      {
                        return std::ref(
                            this->_physicsDispatch.GetFluidProperties()
                                .influence_kernel.GetValue());
                      },
                      details::DetailsType::FLOAT}});
  details.push_back({"Search kernel",
                     {[this]()
                      {
                        return std::ref(
                            this->_physicsDispatch.GetFluidProperties()
                                .search_kernel.GetValue());
                      },
                      details::DetailsType::FLOAT}});
  details.push_back(
      {"Kernel factor A",
       {[this]()
        {
          return std::ref(
              this->_physicsDispatch.GetFluidProperties().kernel_a.GetValue());
        },
        details::DetailsType::FLOAT}});
  details.push_back(
      {"Mass",
       {[this]()
        {
          return std::ref(
              this->_physicsDispatch.GetFluidProperties().mass.GetValue());
        },
        details::DetailsType::FLOAT}});
  details.push_back({"Viscosity factor",
                     {[this]()
                      {
                        return std::ref(
                            this->_physicsDispatch.GetFluidProperties()
                                .viscosity_factor.GetValue());
                      },
                      details::DetailsType::FLOAT}});
  details.push_back(
      {"Rest density",
       {[this]()
        {
          return std::ref(
              this->_physicsDispatch.GetFluidProperties().density0.GetValue());
        },
        details::DetailsType::FLOAT}});
  details.push_back(
      {"Mesh radius",
       {[this]() {
          return std::ref(
              this->_physicsDispatch.GetFluidProperties().mesh_radius);
        },
        details::DetailsType::UINT32}});
  return details;
}

#include "WorldAxes.h"

#include "Essentials.h"

WorldAxes::WorldAxes()
    : Entity(Essentials::PhysicsType::NONE),
      _axes{{{{-1.f, 0.f, 0.f}, {0x00, 0x00, 0x00, 0xFF}},
             {{1.f, 0.f, 0.f}, {0xFF, 0x00, 0x00, 0xFF}}},
            {{{0.f, -1.f, 0.f}, {0x00, 0x00, 0x00, 0xFF}},
             {{0.f, 1.f, 0.f}, {0x00, 0xFF, 0x00, 0xFF}}},
            {{{0.f, 0.f, -1.f}, {0x00, 0x00, 0x00, 0xFF}},
             {{0.f, 0.f, 1.f}, {0x00, 0x00, 0xFF, 0xFF}}}}
{
  for (auto& axis : _axes)
  {
    axis.GetShapeProperties()._scale = {1200, 1200, 1200};
    axis.EnableLight(false);
  }
}
void WorldAxes::Draw() const
{
  Program& renderer =
      _axes[0].GetTesselation() == true
          ? ProgramDispatch::GetInstance()->GetTesselationPipeline()
          : ProgramDispatch::GetInstance()->GetSimplePipeline();

  if (!renderer.isLinked()) renderer.Link();
  renderer.Bind();

  for (auto& axis : _axes)
  {
    axis.Bind(renderer.ID());
    Simulator::GetInstance()->BindUniforms(renderer.ID());
    _physicsDispatch.BindPhysicsMesh(renderer.ID());
    _(glDrawElements(axis.GetDrawPrimitive(), axis.GetVA().Size(),
                     axis.GetVA().IndexBufferType(), nullptr));
    _physicsDispatch.BindPhysicsMesh(renderer.ID());
  }
}

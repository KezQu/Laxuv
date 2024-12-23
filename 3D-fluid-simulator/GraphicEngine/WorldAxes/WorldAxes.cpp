#include "WorldAxes.h"

#include "Essentials.h"

WorldAxes::WorldAxes()
    : Entity(Essentials::PhysicsType::NONE),
      // Specifying X,Y,Z axes with colors match in order R,G,B colors
      _axes{{{{-1.f, 0.f, 0.f}, {0x00, 0x00, 0x00, 0xFF}},
             {{1.f, 0.f, 0.f}, {0xFF, 0x00, 0x00, 0xFF}}},
            {{{0.f, -1.f, 0.f}, {0x00, 0x00, 0x00, 0xFF}},
             {{0.f, 1.f, 0.f}, {0x00, 0xFF, 0x00, 0xFF}}},
            {{{0.f, 0.f, -1.f}, {0x00, 0x00, 0x00, 0xFF}},
             {{0.f, 0.f, 1.f}, {0x00, 0x00, 0xFF, 0xFF}}}}
{
  // Rescalce lines to span throughout simulation world and disable lightning to
  // make axes more visible
  for (auto& axis : _axes)
  {
    axis.GetShapeProperties()._scale = {1200, 1200, 1200};
    axis.EnableLight(false);
  }
}
void WorldAxes::Draw() const
{
  // Choose pipeline based on the fact whether the tesselation is needed to draw
  // axes
  Program& renderer =
      _axes[0].GetTesselation() == true
          ? ProgramDispatch::GetInstance()->GetTesselationPipeline()
          : ProgramDispatch::GetInstance()->GetSimplePipeline();

  // Check if the recompilation and additional linking is needed for the
  // pipeline
  if (!renderer.isLinked()) renderer.Link();
  renderer.Bind();

  // Bind necessary uniforms and buffers to be able to draw valid world axes
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

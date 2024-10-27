#include <WorldAxes.h>

WorldAxes::WorldAxes()
    : Entity(Essentials::PhysicsType::NONE),
      _axes{{{{-1.f, 0.f, 0.f}, {255, 255, 255, 255}},
             {{1.f, 0.f, 0.f}, {0, 0, 255, 255}}},
            {{{0.f, -1.f, 0.f}, {255, 255, 255, 255}},
             {{0.f, 1.f, 0.f}, {0, 255, 0, 255}}},
            {{{0.f, 0.f, -1.f}, {255, 255, 255, 255}},
             {{0.f, 0.f, 1.f}, {255, 0, 0, 255}}}},
      _floor{{{0.f, 0.f, 0.f}, {127, 127, 127, 127}}, 1}
{
  for (auto& axis : _axes)
  {
    axis.GetShapeProperties()._scale = {1200, 1200, 1200};
    axis.EnableLight(false);
  }
  _floor.GetShapeProperties()._scale = {1200, 1200, 1200};
}
void WorldAxes::Draw() const
{
  Program& renderer =
      _axes[0].GetTesselation() == true
          ? ProgramDispatch::GetInstance().GetTesselationPipeline()
          : ProgramDispatch::GetInstance().GetSimplePipeline();

  if (!renderer.isLinked()) renderer.Link();
  renderer.Bind();

  for (auto& axis : _axes)
  {
    axis.Bind(renderer.ID());
    _(glDrawElements(axis.GetDrawPrimitive(), axis.GetVA().Size(),
                     axis.GetVA().IndexBufferType(), nullptr));
  }
  //_floor.Draw();
}

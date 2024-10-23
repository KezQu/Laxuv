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
    axis.GetScale() = {1200, 1200, 1200};
    axis.EnableLight(false);
  }
  _floor.GetScale() = {1200, 1200, 1200};
}
void WorldAxes::Draw() const
{
  for (auto& axis : _axes)
  {
    axis.Bind();
    _(glDrawElements(axis.GetDrawPrimitive(), axis.GetVA().Size(),
                     axis.GetVA().IndexBufferType(), nullptr));
  }
  //_floor.Draw();
}

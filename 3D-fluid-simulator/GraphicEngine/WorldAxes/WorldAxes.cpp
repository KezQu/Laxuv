#include <WorldAxes.h>

WorldAxes::WorldAxes()
	:_axisX{ { { -1.f, 0.f, 0.f }, { 255, 255, 255, 255 } },
			{ { 1.f, 0.f, 0.f }, { 0, 0, 255, 255 } } },
	_axisY{ { { 0.f, -1.f, 0.f }, { 255, 255, 255, 255 } },
			{ { 0.f, 1.f, 0.f }, { 0, 255, 0, 255 } } },
	_axisZ{ { { 0.f, 0.f, -1.f }, { 255, 255, 255, 255 } },
			{ { 0.f, 0.f, 1.f }, { 255, 0, 0, 255 } } },
	_floor{ { { -1.f, 0.f, -1.f }, { 255, 255, 255, 255 } },
			{ { 1.f, 0.f, -1.f }, { 0, 0, 255, 255 } },
			{ { -1.f, 0.f, 1.f }, { 255, 0, 0, 255 } },
			{ { 1.f, 0.f, 1.f }, { 255, 0, 255, 255 } }, }
{
	_axisX.Scale({ 1200, 1200, 1200 });
	_axisY.Scale({ 1200, 1200, 1200 });
	_axisZ.Scale({ 1200, 1200, 1200 });
	_floor.Scale({ 1200, 1200, 1200 });
	_axisX.EnableLight(false);
	_axisY.EnableLight(false);
	_axisZ.EnableLight(false);
}
void WorldAxes::Draw() const {
	_axisY.Draw();
	_axisX.Draw();
	_axisZ.Draw();
	//_floor.Draw();
}


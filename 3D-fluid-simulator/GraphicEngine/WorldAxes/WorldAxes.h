#pragma once

#include <Line.h>
#include <Square.h>

class WorldAxes {
private:
	Line _axisX, _axisY, _axisZ;
	Square _floor;
public:
	WorldAxes();
	void Draw() const;
};

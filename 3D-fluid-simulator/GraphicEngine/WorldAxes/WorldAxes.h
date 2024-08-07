#pragma once

#include <Line.h>
#include <Square.h>
#include <Entity.h>

class WorldAxes : public Entity {
private:
	Line _axes[3];
	Square _floor;
public:
	WorldAxes();
	void Draw() const;
};

#pragma once

#include <Interface.h>

class Toolbar : public Interface {
private:
	static bool _fullscreen;
public:
	Toolbar(ImVec2 const& size, ImVec2 const& position);
	void Generate() override;
};

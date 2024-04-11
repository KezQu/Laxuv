#pragma once

#include <vector>
#include <Interface.h>

class Explorer : public Interface {
public:
	Explorer(ImVec2 const& size, ImVec2 const& position);
	void Generate() override;
	
};

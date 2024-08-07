#pragma once

#include <vector>
#include <unordered_map>
#include <memory>
#include <algorithm>
#include <Interface.h>
#include <Simulator.h>


class Explorer : public Interface {
public:
	Explorer(ImVec2 const& size, ImVec2 const& position);
	void Generate() override;
};
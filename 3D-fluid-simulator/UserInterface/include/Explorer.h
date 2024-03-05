#pragma once

#include <vector>

#include <Interface.h>
//#include <Object.h>

class Explorer : public Interface {
private:
	//static std::vector<Object> ObjectVector;
public:
	Explorer(ImVec2 const& size, ImVec2 const& position);
	void Generate() override;
	
};

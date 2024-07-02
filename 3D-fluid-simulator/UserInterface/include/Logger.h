#pragma once

#include <Interface.h>

#include <deque>
#include <sstream>

class Logger : public Interface{
private:
	static std::deque<std::string> _logHistory;
public:
	Logger(std::ostringstream& log, ImVec2 const& size, ImVec2 const& position);
	void Generate() override;
};
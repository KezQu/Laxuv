#pragma once

#include <Interface.h>
#include <sstream>
#include <chrono>

class Log : public Interface{
private:
	static std::ostringstream LogHistory;
public:
	Log();
	Log(ImVec2 const& size, ImVec2 const& position);
	void Generate() override;
	Log& operator<<(std::string const& LogMessage);
	operator std::ostringstream&();
};

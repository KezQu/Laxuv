#pragma once

#include <Interface.h>
#include <sstream>
#include <chrono>
#include <deque>

template<typename T>
concept is_streamable = requires(std::ostringstream stream, T value) {
	stream << value << std::endl;
};

class Log : public Interface{
private:
	static std::deque<std::ostringstream> logHistory;
public:
	Log();
	Log(ImVec2 const& size, ImVec2 const& position);
	void Generate() override;
	template<is_streamable T>
	std::ostringstream& operator<<(T const& LogMessage);
};

template<is_streamable T>
inline std::ostringstream& Log::operator<<(T const& LogMessage)
{
	auto timestamp = std::chrono::system_clock().now();
	logHistory.back() << "[" << std::format("{:%T}", std::chrono::floor<std::chrono::seconds>(timestamp)) << "] " << LogMessage;
	return logHistory.back();
}
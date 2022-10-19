#pragma once

#include <iostream>
#include <chrono>

class ScopeTimer
{
public:
	ScopeTimer() = delete;
	// outputFormat contains one "{}" where the final time (in seconds) should go
	ScopeTimer(std::string outputFormat);

	~ScopeTimer();

private:
	std::chrono::time_point<std::chrono::high_resolution_clock> startTime;
	std::string outputFormat;
};

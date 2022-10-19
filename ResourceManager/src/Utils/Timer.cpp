#include "Timer.h"

#include "Logger.h"

ScopeTimer::ScopeTimer(std::string _outputFormat)
{
	outputFormat = _outputFormat;
	startTime = std::chrono::high_resolution_clock::now();
}

ScopeTimer::~ScopeTimer()
{
	auto end = std::chrono::high_resolution_clock::now();
	Logger::Info(outputFormat, std::chrono::duration_cast<std::chrono::microseconds>(end - startTime).count() / 1000000.0f);
}

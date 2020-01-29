#pragma once

#include "PROG/parseArgs.h"
#include "PROG/RunTimeInfo.hpp"
#include "PLOG/PlogSetup.h"

namespace CTL::util {

class Program
{
public:
    Program(int argc, char* argv[], plog::Severity debugLevel = plog::debug, bool logToFile = true, bool logToConsole = true);
	void startLog(bool reportArguments=false);
	void endLog(bool reportTimings=false);
	util::RunTimeInfo getRunTimeInfo();
	
protected:
private:
	std::chrono::time_point<std::chrono::steady_clock> start_time;
	util::RunTimeInfo rti;
	int argc;
	char ** argv;
};
} // namespace CTL

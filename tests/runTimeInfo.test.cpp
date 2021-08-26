// Logging, testing
#include "catch.hpp"
#include <plog/Log.h>
#include "PROG/RunTimeInfo.hpp"
#include "PROG/Program.hpp"
using namespace KCT;

TEST_CASE("TEST: RunTimeInfo.hpp", "[RunTimeInfo][Program]")
{
	util::Program prog(0, nullptr);
	util::RunTimeInfo rti;
	std::string exe = rti.getExecutableName();
	std::string path = rti.getExecutablePath();
	std::string dir = rti.getExecutableDirectoryPath();
	REQUIRE(exe.compare("test_ctiol")==0);
}


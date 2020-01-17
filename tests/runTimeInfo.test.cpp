// Logging, testing
#include "catch.hpp"
#include <plog/Log.h>
#include "RunTimeInfo.hpp"
using namespace CTL;

TEST_CASE("TEST: RunTimeInfo.hpp", "RunTimeInfo")
{
	io::RunTimeInfo rti;
	std::string exe = rti.getExecutableName();
	std::string path = rti.getExecutablePath();
	std::string dir = rti.getExecutableDirectoryPath();
	REQUIRE(exe.compare("test_ctiol")==0);
}


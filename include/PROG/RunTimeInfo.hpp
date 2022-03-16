#pragma once

#include <stdexcept>
#include <string>
// Linux specific block
#include <limits.h>
#include <unistd.h>
// Linux specific block
#include "rawop.h"
#include "stringFormatter.h"

namespace KCT::util {
/**
 * @brief Class to provide run time information.
 *
 *	Function can be used as follows
 * @code
 *
 *  #include "RunTimeInfo.hpp"
 *
 *
 *	io::RunTimeInfo rti;
 *	std::string exe = rti.getExecutableName();
 *	std::string path = rti.getExecutablePath();
 *	std::string dir = rti.getExecutableDirectoryPath();
 *	std::cout <<	io::xprintf("Executable is %s", exe.c_str());
 *	std::cout << io::xprintf("Executable path is %s", path.c_str());
 *	std::cout << io::xprintf("Executable dir is %s", dir.c_str());
 * @endcode
 */
class RunTimeInfo
{
public:
    RunTimeInfo(int argc, char* argv[]);
    RunTimeInfo();
    std::string getExecutablePath();
    std::string getExecutableName();
    std::string getExecutableDirectoryPath();

private:
    int argc;
    char** argv;
    std::string executablePath;
};
} // namespace KCT::util

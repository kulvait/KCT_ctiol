#include "PROG/RunTimeInfo.hpp"

namespace KCT {
namespace util {
    RunTimeInfo::RunTimeInfo()
        : RunTimeInfo(0, nullptr)
    {
    }

    RunTimeInfo::RunTimeInfo(int argc, char* argv[])
        : argc(argc)
        , argv(argv)
    {
        char exepath[PATH_MAX + 1] = { 0 };
        ssize_t len = ::readlink("/proc/self/exe", exepath, PATH_MAX);
        if(len != -1)
        {
            exepath[len] = 0;
            executablePath = std::string(exepath);
        } else
        {
            throw std::runtime_error(io::xprintf("Can not read process path."));
        }
    }

    std::string RunTimeInfo::getExecutablePath() { return (executablePath); }

    std::string RunTimeInfo::getExecutableName() { return io::getBasename(executablePath); }

    std::string RunTimeInfo::getExecutableDirectoryPath() { return io::getParent(executablePath); }

} // namespace io
} // namespace KCT

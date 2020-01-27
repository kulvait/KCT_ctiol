#include "PROG/Program.hpp"

namespace CTL::util {

Program::Program(
    int argc, char* argv[], plog::Severity debugLevel, bool logToFile, bool logToConsole)
    : argc(argc)
    , argv(argv)
{
    plog::Severity verbosityLevel = debugLevel; // debug, info, ...
    rti = util::RunTimeInfo(argc, argv);
    std::string csvLogFile = "";
    if(logToFile)
    {
        std::string csvLogFile = io::xprintf(
            "/tmp/%s.csv", rti.getExecutableName().c_str()); // Set to empty string to disable
    }
    std::string xpath = rti.getExecutableDirectoryPath();
    plog::PlogSetup plogSetup(verbosityLevel, csvLogFile, logToConsole);
    plogSetup.initLogging();
    start_time = std::chrono::steady_clock::now();
}

void Program::startLog() { LOGI << io::xprintf("START %s", rti.getExecutableName().c_str()); }

void Program::endLog(bool reportTimings)
{
    if(reportTimings)
    {
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::steady_clock::now() - start_time);
        LOGI << io::xprintf("END %s, duration %0.3f s.", rti.getExecutableName().c_str(),
                            float(duration.count() / 1000.0f));
    } else
    {
        LOGI << io::xprintf("END %s", rti.getExecutableName().c_str());
    }
}

util::RunTimeInfo Program::getRunTimeInfo() { return rti; }

} // namespace CTL::util

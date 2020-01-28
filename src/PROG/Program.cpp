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
    using namespace std::chrono;
    if(reportTimings)
    {
        auto t = std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::steady_clock::now() - start_time);
        auto min = duration_cast<minutes>(t);
        t -= duration_cast<milliseconds>(min);
        auto h = duration_cast<hours>(min);
        min -= duration_cast<minutes>(h);
        if(h.count() > 0)
        {
            LOGI << io::xprintf("END %s, duration %02dh %02dm %02.3fs.",
                                rti.getExecutableName().c_str(), h.count(), min.count(),
                                float(t.count() / 1000.0f));
        } else if(min.count() > 0)
        {
            LOGI << io::xprintf("END %s, duration %02dm %02.3fs.",
                                rti.getExecutableName().c_str(), min.count(),
                                float(t.count() / 1000.0f));
        } else
        {
            LOGI << io::xprintf("END %s, duration %02.3fs.", rti.getExecutableName().c_str(),
                                float(t.count() / 1000.0f));
        }
    } else
    {
        LOGI << io::xprintf("END %s", rti.getExecutableName().c_str());
    }
}

util::RunTimeInfo Program::getRunTimeInfo() { return rti; }

} // namespace CTL::util

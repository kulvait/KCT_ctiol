#define CATCH_CONFIG_RUNNER
#include "catch2/catch.hpp"
#include "PlogSetup.h"

using namespace CTL;

int main(int argc, char* argv[])
{
    // global setup...
    bool logging = true;
    if (logging) {
        plog::Severity verbosityLevel = plog::debug; //Set to debug to see the debug messages, info messages
        std::string csvLogFile = "/tmp/imageRegistrationLog.csv"; //Set NULL to disable
        bool logToConsole = true;
        util::PlogSetup plogSetup(verbosityLevel, csvLogFile, logToConsole);
        plogSetup.initLogging();
    }
    int result = Catch::Session().run(argc, argv);

    // global clean-up...

    return result;
}

TEST_CASE("LOGGING SETUP", "catch_main.cpp")
{
}

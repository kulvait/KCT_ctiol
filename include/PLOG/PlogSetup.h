#pragma once
// Capture file name when logging
#define PLOG_CAPTURE_FILE

#include "PLOG/PlogFormatter.h"
#include <plog/Appenders/ColorConsoleAppender.h>
#include <plog/Log.h>

#include <string>

namespace plog {
    /**Class for convenience to init logging using PLOG logger.
     *
     */
    class PlogSetup
    {
    public:
        /**
         *Constructs and setup class PlogSetup.
         *
         *\param verbosityLevel is one of the following values none, fatal, error, warning, info,
         *debug, verbose \param csvLogFile if empty string is supplied or parameter is ommitted,
         *logging to file is disabled \param logToConsole if false then logging to console is
         *disabled
         */
        PlogSetup(plog::Severity verbosityLevel,
                  std::string csvLogFile = "",
                  bool logToConsole = true)
            : verbosityLevel(verbosityLevel)
            , csvLogFile(csvLogFile)
            , logToConsole(logToConsole)
        {
        }
        /**
         *\brief Logging will be configurred only after this function is called.
         */
        void initLogging()
        {
            if(!csvLogFile.empty())
            {
                static plog::RollingFileAppender<plog::CsvFormatter> fileAppender(
                    csvLogFile.c_str(), 0, 0);
                if(logToConsole)
                {
                    static plog::ColorConsoleAppender<plog::PlogFormatter> consoleAppender;
                    plog::init(verbosityLevel, &fileAppender).addAppender(&consoleAppender);
                } else
                {
                    plog::init(verbosityLevel, &fileAppender);
                }
            } else
            {
                if(logToConsole)
                {
                    static plog::ColorConsoleAppender<plog::PlogFormatter> consoleAppender;
                    plog::init(verbosityLevel, &consoleAppender);
                }
            }
            return;
        }

    private:
        plog::Severity verbosityLevel; // Set to debug to see the debug messages, info messages
        std::string csvLogFile; // Set to empty string to disable
        bool logToConsole;
    };

} // namespace plog

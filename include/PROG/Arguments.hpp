#pragma once

#include "CLI/CLI.hpp" //Command line parser
#include "PROG/parseArgs.h"

namespace CTL::util {

class Arguments
{
public:
    Arguments(int argc, char** argv, std::string appName = "");
    std::shared_ptr<CLI::App> getCliApp();
    // Call this function for parsing
/**
* @brief 
*
* @param helpOnError Whether to print a help message on parsing error.
*
* @return 
*/
    int parse(bool helpOnError = true);
    virtual void defineArguments() = 0;
    virtual int preParse() = 0;
    virtual int postParse() = 0;

protected:
    std::shared_ptr<CLI::App> cliApp;
    int argc;
    char** argv;
};
} // namespace CTL::util

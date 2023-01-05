#pragma once

#include "CLI/CLI.hpp" //Command line parser
#include "PROG/parseArgs.h"
#include "PROG/KCTException.hpp"

namespace KCT::util {

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
    virtual ~Arguments() = default;

    void registerOption(std::string optName, CLI::Option* opt);
    void registerOptionGroup(std::string optGroupName, CLI::Option_group* og);
    CLI::Option_group* getRegisteredOptionGroup(std::string optGroupName);
    CLI::Option* getRegisteredOption(std::string optName);

protected:
    std::shared_ptr<CLI::App> cliApp;
    std::map<std::string, CLI::Option_group*> cliOptionGroups;
    std::map<std::string, CLI::Option*> cliOptions;
    int argc;
    char** argv;
    std::string prgInfo;
};
} // namespace KCT::util

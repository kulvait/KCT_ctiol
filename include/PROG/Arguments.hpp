#pragma once

#include "PROG/parseArgs.h"
#include "CLI/CLI.hpp" //Command line parser

namespace CTL::util {

class Arguments
{
public:
    Arguments(int argc, char ** argv, std::string appName="");
    std::shared_ptr<CLI::App> getCliApp();
	//Call this function for parsing
	int parse();
	virtual void defineArguments() = 0;
	virtual int preParse() = 0;
	virtual int postParse() = 0;

protected:
    std::shared_ptr<CLI::App> cliApp;
	int argc;
	char ** argv;
};
} // namespace CTL

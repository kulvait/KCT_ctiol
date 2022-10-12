#include "PROG/ArgumentsVerbose.hpp" //Command line parser

using namespace KCT;
using namespace KCT::util;

void ArgumentsVerbose::addVerboseArgs(const std::string& helpString)
{

    registerOption("verbose",
                   cliApp->add_flag("--verbose", verbose,
                                    io::xprintf("%s, defaults to %s.", helpString.c_str(),
                                                verbose ? "true" : "false")));
}

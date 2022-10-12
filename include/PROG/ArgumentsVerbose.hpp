#pragma once

#include "CLI/CLI.hpp" //Command line parser
#include "PROG/Arguments.hpp"

namespace KCT::util {
// Virtual inheritance to have only one instance of Arguments in the pedigree
class ArgumentsVerbose : public virtual Arguments
{
public:
    ArgumentsVerbose(int argc, char** argv, std::string prgName)
        : Arguments(argc, argv, prgName){};

    bool verbose = false;

protected:
    const static inline std::string defaultHelpString = "Increase verbosity of the program";
    void addVerboseArgs(const std::string &helpString = defaultHelpString);
};

} // namespace KCT::util

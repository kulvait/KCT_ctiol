#pragma once

#include "CLI/CLI.hpp" //Command line parser
#include "PROG/Arguments.hpp"

namespace CTL::util {
// Virtual inheritance to have only one instance of Arguments in the pedigree
class ArgumentsForce : public virtual Arguments
{
public:
    ArgumentsForce(int argc, char** argv, std::string prgName)
        : Arguments(argc, argv, prgName){};

    bool force = false;

protected:
    void addForceArgs();
};

} // namespace CTL::util

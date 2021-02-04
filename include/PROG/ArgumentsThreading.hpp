#pragma once

#include "CLI/CLI.hpp" //Command line parser
#include "PROG/Arguments.hpp"

namespace CTL::util {
class ArgumentsThreading : public virtual Arguments
{
public:
    ArgumentsThreading(int argc, char** argv, std::string prgName)
        : Arguments(argc, argv, prgName){};

    uint32_t threads = 0;

protected:
    void addThreadingArgs(CLI::Option_group* og = nullptr);
};

} // namespace CTL::util

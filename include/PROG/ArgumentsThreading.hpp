#pragma once
#include <string>
#include <thread>

#include "CLI/CLI.hpp" //Command line parser
#include "PROG/Arguments.hpp"

namespace KCT::util {
class ArgumentsThreading : public virtual Arguments
{
public:
    ArgumentsThreading(int argc, char** argv, std::string prgName)
        : Arguments(argc, argv, prgName)
    {
        numProcessors = std::thread::hardware_concurrency();
        threads = numProcessors; // Set the default value to the number of processors
    }

    uint32_t threads = 0;

protected:
    void addThreadingArgs(CLI::Option_group* og = nullptr);

private:
    uint32_t numProcessors = 0;
};

} // namespace KCT::util

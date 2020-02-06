#include "PROG/ArgumentsThreading.hpp" //Command line parser

using namespace CTL;
using namespace CTL::util;

void ArgumentsThreading::addThreadingArgs()
{

    registerOption(
        "threads",
        cliApp
            ->add_option("-j,--threads", threads,
                         io::xprintf("Number of extra threads that the program can use, default is "
                                     "%d. Zero extra threads means no threading.",
                                     threads))
            ->check(CLI::Range(0, 65535)));
}

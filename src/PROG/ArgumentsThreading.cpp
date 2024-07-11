#include "PROG/ArgumentsThreading.hpp" //Command line parser

using namespace KCT;
using namespace KCT::util;

void ArgumentsThreading::addThreadingArgs(CLI::Option_group* og)
{
    CLI::Option* threads_opt;
    std::string spec = "-j,--threads";
    std::string help
        = io::xprintf("Number of extra threads that the program can use, default is %d. "
                      "Zero value denotes no threading. Negative values represent fractions "
                      "of cores, e.g., -1 means use %d cores, -2 means use %d cores, etc.",
                      threads, numProcessors, numProcessors / 2);

    auto thread_callback = [this](const int32_t& value) {
        if(value < 0)
        {
            threads = numProcessors / -value;
        } else
        {
            threads = value;
        }
        if(threads > 10 * numProcessors)
        {
            threads = 10 * numProcessors;
        }
        LOGI << io::xprintf("Using %d threads", threads);
    };

    if(og == nullptr)
    {
        threads_opt = cliApp->add_option_function<int32_t>(spec, thread_callback, help);
    } else
    {
        threads_opt = og->add_option_function<int32_t>(spec, thread_callback, help);
    }

    threads_opt->check(CLI::Range(-10, static_cast<int32_t>(numProcessors * 10)));
    registerOption("threads", threads_opt);
}

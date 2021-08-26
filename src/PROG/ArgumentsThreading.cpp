#include "PROG/ArgumentsThreading.hpp" //Command line parser

using namespace KCT;
using namespace KCT::util;

void ArgumentsThreading::addThreadingArgs(CLI::Option_group* og)
{
    CLI::Option* threads_opt;
	std::string spec = "-j,--threads";
	std::string help = 
            io::xprintf("Number of extra threads that the program can use, default is "
                        "%d. Zero value denotes no threading.",
                        threads);
    if(og == nullptr)
    {
        threads_opt = cliApp->add_option( spec, threads, help);
    }else
	{
        threads_opt = og->add_option( spec, threads, help);
	}
    threads_opt->check(CLI::Range(0, 65535));
	registerOption("threads", threads_opt);	
}

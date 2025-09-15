#include "PROG/ArgumentsThreadingCUDA.hpp"

using namespace KCT;
using namespace KCT::util;

void ArgumentsThreadingCUDA::addThreadingArgs(CLI::Option_group* og) 
{
    CLI::Option* threads_opt;
    std::string spec = "-j,--threads";
    std::string help;

    // Adjust the input range for CLI. Negative values allowed for fractions (inputRangeMin)
    int32_t inputRangeMin = -10;  // Minimum allowed value for input (negative for fractions)
    uint32_t inputRangeMax;       // Maximum allowed value for input (positive only)

    // If strictThreadRange is set, cap the max thread count to the number of GPUs.
    if(strictThreadRange)
    {   
        inputRangeMax = static_cast<uint32_t>(numGPUs); // Max threads are capped by number of GPUs
    }   
    else 
    {   
        // Relaxed case: allow up to 10 times the number of GPUs.
        inputRangeMax = static_cast<uint32_t>(numGPUs * 10);
    }   

    // Help message reflecting the new range and explaining negative values
    help = io::xprintf(
        "Number of threads that the program can use, default is the number of CUDA devices %d. "
        "Value must be in the range [%d, %d]. Negative values represent "
        "fractions of GPU devices (e.g., -1 means use %d GPUs, -2 means use %d GPUs).",
        threads, inputRangeMin, inputRangeMax, numGPUs, numGPUs / 2); 

    // Callback to process the value of `--threads`
    auto thread_callback = [this, inputRangeMax](const int32_t& value) {
        if(value < 0)
        {   
            // Negative value means fraction of GPU devices
            threads = numGPUs / -value; 
        }   
        else
        {   
            // Positive value means direct thread count
            threads = value;
        }   

        // Cap the thread count at the allowed maximum
        if(threads > inputRangeMax)
        {   
            threads = inputRangeMax;
        }   

        LOGI << io::xprintf("Using %d threads", threads);
    };  

    // Add the CLI option
    if(og == nullptr)
    {   
        threads_opt = cliApp->add_option_function<int32_t>(spec, thread_callback, help);
    }   
    else 
    {   
        threads_opt = og->add_option_function<int32_t>(spec, thread_callback, help);
    }   

    // Set the range based on the input range
    threads_opt->check(CLI::Range(inputRangeMin, static_cast<int32_t>(inputRangeMax)));

    // Register the option
    registerOption("threads", threads_opt);
}

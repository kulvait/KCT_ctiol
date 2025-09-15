#pragma once
#include "ArgumentsThreadingI.hpp" // Include the ArgumentsThreadingI interface
#include "CLI/CLI.hpp" // Command line parser
#include <string>
#include <thread>

namespace KCT::util {

/**
 * @brief Implementation of ArgumentsThreading for CPU-based threading.
 *
 * This class implements the ArgumentsThreadingI interface and provides the logic
 * for adding threading-related arguments to the command-line interface and
 * managing threading configuration for CPU-based threading models.
 */
class ArgumentsThreading : public ArgumentsThreadingI
{
public:
    /**
     * @brief Constructor for ArgumentsThreading, initializes threading arguments.
     *
     * In this constructor, we calculate the default number of threads based on
     * the number of processors available. The `threads` value will be updated
     * to reflect this number. The `strictThreadRange` flag controls whether the
     * user can specify values outside the range of `[0, numProcessors]`.
     *
     * @param argc Command-line argument count.
     * @param argv Command-line argument values.
     * @param prgName The name of the program.
     * @param strictThreadRange If true, limits thread count to the range `[0, numProcessors]`.
     *                           If false, allows values in the range `[-10, numProcessors * 10]`.
     */
    ArgumentsThreading(int argc, char** argv, std::string prgName, const bool strictThreadRange = false)
        : ArgumentsThreadingI(argc, argv, prgName) // Chain to the ArgumentsThreadingI constructor
    {
        numProcessors = std::thread::hardware_concurrency(); // Get number of processors
        threads = numProcessors; // Default to the number of processors
        this->strictThreadRange = strictThreadRange; // Store the flag
    }

    ~ArgumentsThreading() override = default; // Default destructor

    /**
     * @brief Adds command-line arguments for configuring threading behavior.
     *
     * This function adds the `--threads` option to the command-line interface. It
     * allows the user to specify the number of threads to use for processing. The
     * user input is processed and the `threads` variable is updated accordingly.
     *
     * @param og Optional parameter that allows specifying a group for the options.
     */
    void addThreadingArgs(CLI::Option_group* og = nullptr) override;


private:
    uint32_t numProcessors = 0; /**< Number of available processors. */
    bool strictThreadRange = false; /**< Flag to enforce strict thread range. */

};

} // namespace KCT::util

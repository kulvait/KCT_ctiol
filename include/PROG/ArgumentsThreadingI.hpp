#pragma once
#include "CLI/CLI.hpp" // Command Line Interface library
#include <string>

#include "PROG/Arguments.hpp"

namespace KCT::util {
/**
 * @brief Interface for defining threading behavior and arguments.
 *
 * This abstract class provides the structure for handling threading configurations,
 * such as adding CLI arguments and retrieving thread count. It allows derived classes
 * to implement different threading models (e.g., CPU or GPU threading) with specific defaults.
 *
 * @note This is an abstract class and cannot be instantiated directly.
 */
class ArgumentsThreadingI : public virtual Arguments
{
public:
    /** 
     * @brief Constructor initializes threading settings.
     *
     * Sets a default value for `threads` (1 by default), and chains to the base
     * `Arguments` constructor for argument parsing.
     *
     * @param argc Command-line argument count.
     * @param argv Command-line argument values.
     * @param prgName The name of the program.
     */
    ArgumentsThreadingI(int argc, char** argv, std::string prgName)
        : Arguments(argc, argv, prgName)  // Chain to base constructor
    {   
        threads = 1;  // Default value of threads: 1 (single-threaded)
    }

    /** 
     * @brief Adds threading-related arguments to the CLI.
     *
     * To be implemented by derived classes, allowing them to add threading options
     * like `--threads` or `--gpu-threads` based on their execution model.
     */
    virtual void addThreadingArgs(CLI::Option_group* og = nullptr) = 0;

    /** 
     * @brief Virtual method to retrieve the number of threads to be used.
     *
     * This method retrieves the number of threads to be used for processing. The default
     * behavior is to return the value of `threads`. Derived classes can override this method
     * if more dynamic or customized thread-count logic is required.
     *
     * @return The number of threads to use for the program.
     */
    virtual uint32_t getThreadCount() const { return threads; }


    /** 
     * @brief Virtual destructor.
     *
     * Ensures proper destruction of derived class objects when using base class pointers.
     */
    virtual ~ArgumentsThreadingI() = default;

    uint32_t threads; /**< Number of threads to use (default is 1). Derived classes can modify this. */

protected:
    /** 
     * @brief Protected constructor ensures this class can't be instantiated directly.
     *
     * Designed for use by derived classes, which will handle threading argument logic.
     */
    ArgumentsThreadingI() = default;
};

} // namespace KCT::util

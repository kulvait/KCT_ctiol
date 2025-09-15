#pragma once
#include "ArgumentsThreadingI.hpp" // Include the ArgumentsThreadingI interface
#include "CLI/CLI.hpp" // Command line parser
#include "OPENCL/OpenCLManager.hpp" // OpenCL manager for platform/device queries
#include <string>

namespace KCT::util {

/**
 * @brief Implementation of ArgumentsThreading for OpenCL-based threading.
 *
 * This class implements the ArgumentsThreadingI interface and provides the logic
 * for adding threading-related arguments to the command-line interface and
 * managing threading configuration for OpenCL-based threading models (GPU).
 */
class ArgumentsThreadingOpenCL : public ArgumentsThreadingI
{
public:
    /**
     * @brief Constructor for ArgumentsThreadingOpenCL, initializes threading arguments.
     *
     * In this constructor, we calculate the default number of threads based on
     * the number of available OpenCL devices. The `threads` value will be updated
     * to reflect this number. The `strictThreadRange` flag controls whether the
     * user can specify values outside the range of `[0, numGPUs]`.
     *
     * @param argc Command-line argument count.
     * @param argv Command-line argument values.
     * @param prgName The name of the program.
     * @param strictThreadRange If true, limits thread count to the range `[0, numGPUs]`.
     *                           If false, allows values in the range `[-10, numGPUs * 10]`.
     */
    ArgumentsThreadingOpenCL(int argc,
                             char** argv,
                             std::string prgName,
                             const bool strictThreadRange = false)
        : ArgumentsThreadingI(argc, argv, prgName) // Chain to the ArgumentsThreadingI constructor
    {
        // Query number of OpenCL platforms and devices
        std::optional<std::pair<uint32_t, uint32_t>> platform_device
            = OpenCLManager::chooseSuitablePlatformAndDevice();
        if(platform_device.has_value())
        {
            uint32_t platformID = platform_device->first;
            numGPUs
                = OpenCLManager::deviceCount(platformID); // Get number of GPUs from OpenCLManager
        } else
        {
            numGPUs = 0; // No GPUs found by default
        }

        threads = numGPUs; // Default to the number of GPUs
        this->strictThreadRange = strictThreadRange; // Store the flag
    }

    ~ArgumentsThreadingOpenCL() override = default; // Default destructor

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
    uint32_t numGPUs; /**< Number of available OpenCL-enabled GPUs. */
    bool strictThreadRange; /**< Flag to enforce strict thread range. */
};

} // namespace KCT::util

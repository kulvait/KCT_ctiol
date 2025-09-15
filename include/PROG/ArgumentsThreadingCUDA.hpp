#pragma once
#include "ArgumentsThreadingI.hpp" // Include the ArgumentsThreadingI interface
#include "CLI/CLI.hpp" // Command line parser
#include <cuda_runtime.h> // CUDA runtime header for device queries
#include <string>

namespace KCT::util {

/**
 * @brief Implementation of ArgumentsThreading for CUDA-based threading.
 *
 * This class implements the ArgumentsThreadingI interface and provides the logic
 * for adding threading-related arguments to the command-line interface and
 * managing threading configuration for CUDA-based threading models (GPU).
 */
class ArgumentsThreadingCUDA : public ArgumentsThreadingI
{
public:
    /**
     * @brief Constructor for ArgumentsThreadingCUDA, initializes threading arguments.
     *
     * In this constructor, we calculate the default number of threads based on
     * the number of available GPU devices. The `threads` value will be updated
     * to reflect this number. The `strictThreadRange` flag controls whether the
     * user can specify values outside the range of `[0, numGPUs]`.
     *
     * @param argc Command-line argument count.
     * @param argv Command-line argument values.
     * @param prgName The name of the program.
     * @param strictThreadRange If true, limits thread count to the range `[0, numGPUs]`.
     *                           If false, allows values in the range `[-10, numGPUs * 10]`.
     */
    ArgumentsThreadingCUDA(int argc,
                           char** argv,
                           std::string prgName,
                           const bool strictThreadRange = false)
        : ArgumentsThreadingI(argc, argv, prgName) // Chain to the ArgumentsThreadingI constructor
    {
        int deviceCount = 0;
        cudaError_t err = cudaGetDeviceCount(&deviceCount); // Query number of CUDA devices
        if(err != cudaSuccess)
        {
            // Get the error string
            const char* cudaErr = cudaGetErrorString(err);
            // Construct the full error message by concatenating the strings
            std::string errMsg = "CUDA Error: " + std::string(cudaErr);
            // Log the error using KCTERR (you can replace this with actual logging code)
            KCTERR(errMsg);
        }
        numGPUs = static_cast<uint32_t>(deviceCount); // Default to the number of GPUs available
        threads = numGPUs; // Default to the number of GPUs
        this->strictThreadRange = strictThreadRange; // Store the flag
    }

    ~ArgumentsThreadingCUDA() override = default; // Default destructor

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
    uint32_t numGPUs; /**< Number of available CUDA-enabled GPUs. */
    bool strictThreadRange; /**< Flag to enforce strict thread range. */
};

} // namespace KCT::util

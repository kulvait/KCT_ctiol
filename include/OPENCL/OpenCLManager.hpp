#pragma once

// Logging
#include "PLOG/PlogSetup.h"

// External libraries
#include <optional>
#include <CL/cl.hpp>

// Internal libraries
#include "stringFormatter.h"

namespace KCT {
namespace util {
    class OpenCLManager
    {
    public:
        static uint32_t platformCount();
        static std::shared_ptr<cl::Platform> getPlatform(uint32_t platformID = 0,
                                                         bool verbose = false);
        static std::string getPlatformName(uint32_t platformID);
        static uint32_t deviceCount(uint32_t platformID);
        static std::shared_ptr<cl::Device>
        getDevice(const cl::Platform& platform, uint32_t deviceID = 0, bool verbose = false);
        static uint64_t localMemSize(cl::Device& device);
        static uint64_t localMemSize(uint32_t platformID, uint32_t deviceID);
        static uint32_t maxWGS(cl::Device& device);
        static uint32_t maxWGS(uint32_t platformID, uint32_t deviceID);
        // New function to choose a platform and device
        static std::optional<std::pair<uint32_t, uint32_t>> chooseSuitablePlatformAndDevice(bool verbose
                                                                                    = false);
    };
} // namespace util
} // namespace KCT

#pragma once

// Logging
#include "PLOG/PlogSetup.h"

// External libraries
#include <CL/cl.hpp>

// Internal libraries
#include "stringFormatter.h"

namespace CTL {
namespace util {
    class OpenCLManager
    {
    public:
        static uint32_t platformCount();
        static std::shared_ptr<cl::Platform> getPlatform(uint32_t platformID = 0, bool verbose = false);
        static uint32_t deviceCount(uint32_t platformID);
        static std::shared_ptr<cl::Device>
        getDevice(const cl::Platform& platform, uint32_t deviceID = 0, bool verbose = false);
    };
} // namespace util
} // namespace CTL

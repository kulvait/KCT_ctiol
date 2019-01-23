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
        std::shared_ptr<cl::Platform> getPlatform(uint32_t id = 0, bool verbose = false);
        std::shared_ptr<cl::Device> getDevice(const cl::Platform& platform, uint32_t id = 0, bool verbose = false);
    };
} // namespace util
} // namespace CTL

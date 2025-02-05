#pragma once

// Logging
#include "PLOG/PlogSetup.h"

// External libraries
#include <CL/cl.hpp>
#include <optional>

// Internal libraries
#include "stringFormatter.h"

namespace KCT {
namespace util {
    class OpenCLManager
    {
    public:
        // Basic functions to count platforms and devices
        static uint32_t platformCount();
        static uint32_t deviceCount(uint32_t platformID);
        static std::string getPlatformName(uint32_t platformID);
        static bool isValid(uint32_t platformID, uint32_t deviceID);
        static std::shared_ptr<cl::Platform> getPlatform(uint32_t platformID = 0,
                                                         bool verbose = false);
        static std::shared_ptr<cl::Device>
        getDevice(const cl::Platform& platform, uint32_t deviceID = 0, bool verbose = false);
        static uint64_t localMemSize(cl::Device& device);
        static uint64_t localMemSize(uint32_t platformID, uint32_t deviceID);
        static uint32_t maxWGS(cl::Device& device);
        static uint32_t maxWGS(uint32_t platformID, uint32_t deviceID);
        // Choose platform and device
        static std::optional<std::pair<uint32_t, uint32_t>>
        chooseSuitablePlatformAndDevice(const std::string& platformNameFilter = "",
                                        bool verbose = false);
        static std::optional<uint32_t> getPlatformID(const std::string& platformNameFilter,
                                                     bool verbose = false);

    private:
        static const std::vector<std::string> platformPriorityList; // Priority list
    };
} // namespace util
} // namespace KCT

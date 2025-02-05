#include "OPENCL/OpenCLManager.hpp"

namespace KCT {
namespace util {
    uint32_t OpenCLManager::platformCount()
    {
        std::vector<cl::Platform> all_platforms;
        cl::Platform::get(&all_platforms);
        return all_platforms.size();
    }

    uint32_t OpenCLManager::deviceCount(uint32_t platformID)
    {
        std::vector<cl::Device> all_devices;
        std::shared_ptr<cl::Platform> platform = getPlatform(platformID);
        if(platform == nullptr)
        {
            return 0;
        } else
        {
            platform->getDevices(CL_DEVICE_TYPE_ALL, &all_devices);
            return all_devices.size();
        }
    }

    bool OpenCLManager::isValid(uint32_t platformID, uint32_t deviceID)
    {
        // Check if the platform ID is within the valid range
        if(platformID >= platformCount())
        {
            return false; // Invalid platform ID
        }

        // Check if the device ID is within the valid range for the given platform
        if(deviceID >= deviceCount(platformID))
        {
            return false; // Invalid device ID
        }

        return true; // Both platformID and deviceID are valid
    }

    std::optional<std::pair<uint32_t, uint32_t>>
    OpenCLManager::chooseSuitablePlatformAndDevice(const std::string& platformNameFilter,
                                                   bool verbose)
    {
        uint32_t platformsOpenCL = platformCount();
        if(platformsOpenCL == 0)
        {
            LOGE << "No OpenCL platforms found. Check OpenCL installation!";
            return std::nullopt;
        }

        uint32_t CLplatformID = 0;
        uint32_t CLdeviceID = 0;
        uint32_t devicesOnPlatform = 0;

        // First pass: Filter by platformNameFilter and prioritize
        for(const std::string& priorityPlatform : platformPriorityList)
        {
            for(uint32_t platformID = 0; platformID < platformsOpenCL; platformID++)
            {
                std::string platformName = getPlatformName(platformID);

                if(priorityPlatform == platformName)
                {
                    // If platformNameFilter is non-empty, ensure the platform matches the filter
                    if(!platformNameFilter.empty()
                       && platformName.find(platformNameFilter) == std::string::npos)
                    {
                        continue; // Skip platforms that don't match the filter
                    }
                    devicesOnPlatform = deviceCount(platformID);
                    if(devicesOnPlatform > 0)
                    {
                        CLplatformID = platformID;
                        CLdeviceID = devicesOnPlatform - 1; // Select the last device
                        if(verbose)
                        {
                            LOGI << io::xprintf("Selected device %d:%d on platform %s.",
                                                CLplatformID, CLdeviceID, platformName.c_str());
                        }
                        return std::make_pair(CLplatformID, CLdeviceID);
                    }
                }
            }
        }

        // Second pass: search all platform and prioritize first one with most devices
        for(uint32_t platformID = 0; platformID < platformsOpenCL; platformID++)
        {
            std::string platformName = getPlatformName(platformID);

            // If platformNameFilter is non-empty, ensure the platform matches the filter
            if(!platformNameFilter.empty()
               && platformName.find(platformNameFilter) == std::string::npos)
            {
                continue; // Skip platforms that don't match the filter
            }
            uint32_t devicesCount = deviceCount(platformID);
            if(devicesCount > devicesOnPlatform)
            {
                devicesOnPlatform = devicesCount;
                CLplatformID = platformID;
                CLdeviceID = devicesOnPlatform - 1; // Select the last device
            }
        }

        if(devicesOnPlatform == 0)
        {
            LOGE << "No devices found on any platform.";
            return std::nullopt;
        }

        if(verbose)
        {
            std::string platformName = getPlatformName(CLplatformID);
            LOGI << io::xprintf("Selected device %d:%d on platform %s.", CLplatformID, CLdeviceID,
                                platformName.c_str());
        }

        return std::make_pair(CLplatformID, CLdeviceID);
    }

    std::optional<uint32_t> OpenCLManager::getPlatformID(const std::string& platformNameFilter,
                                                         bool verbose)
    {
        uint32_t platformsOpenCL = platformCount();
        uint32_t ID = 0;
        uint32_t matchCount = 0;
        for(uint32_t platformID = 0; platformID < platformsOpenCL; platformID++)
        {
            std::string platformName = getPlatformName(platformID);

            // If platformNameFilter is non-empty, ensure the platform matches the filter
            if(platformName.find(platformNameFilter) != std::string::npos)
            {
                matchCount++;
                ID = platformID;
            }
        }
        if(matchCount == 1)
        {
            return ID;
        }
        std::string ERR = io::xprintf("Searching for platformNameFilter=%s got me %d results!",
                                      platformNameFilter.c_str(), matchCount);
        LOGE << ERR;
        return std::nullopt;
    }

    std::shared_ptr<cl::Platform> OpenCLManager::getPlatform(uint32_t platformID, bool verbose)
    {
        std::vector<cl::Platform> all_platforms;
        cl::Platform::get(&all_platforms);
        if(all_platforms.size() == 0)
        {
            LOGE << "No platforms found. Check OpenCL installation!";
            return nullptr;
        }
        if(all_platforms.size() <= platformID)
        {
            LOGE << io::xprintf("There is just %d OpenCL platforms on the PC and you selected "
                                "invalid zero based index %d.",
                                all_platforms.size(), platformID);
            return nullptr;
        }
        if(verbose)
        {
            LOGD << io::xprintf("There exists %d OpenCL platforms on the PC.",
                                all_platforms.size());
            LOGI << io::xprintf("Selected OpenCL platform %d: %s.", platformID,
                                all_platforms[platformID].getInfo<CL_PLATFORM_NAME>().c_str());
        }
        return std::make_shared<cl::Platform>(all_platforms[platformID]);
    }

    std::string OpenCLManager::getPlatformName(uint32_t platformID)
    {
        std::vector<cl::Platform> all_platforms;
        cl::Platform::get(&all_platforms);
        if(all_platforms.size() <= platformID)
        {
            LOGE << io::xprintf("There is just %d OpenCL platforms on the PC and you selected "
                                "invalplatformID zero based index %d.",
                                all_platforms.size(), platformID);
            return nullptr;
        }
        return all_platforms[platformID].getInfo<CL_PLATFORM_NAME>().c_str();
    }

    uint64_t OpenCLManager::localMemSize(cl::Device& device)
    {
        cl_ulong locsize;
        device.getInfo(CL_DEVICE_LOCAL_MEM_SIZE, &locsize);
        return static_cast<uint64_t>(locsize);
    }

    uint64_t OpenCLManager::localMemSize(uint32_t platformID, uint32_t deviceID)
    {
        std::shared_ptr<cl::Platform> platform = getPlatform(platformID);
        if(platform == nullptr)
        {
            return 0;
        } else
        {
            std::shared_ptr<cl::Device> dev = getDevice(*platform, deviceID);
            if(dev == nullptr)
            {
                return 0;
            } else
            {
                return localMemSize(*dev);
            }
        }
    }

    uint32_t OpenCLManager::maxWGS(cl::Device& device)
    {
        size_t val;
        device.getInfo(CL_DEVICE_MAX_WORK_GROUP_SIZE, &val);
        return static_cast<uint64_t>(val);
    }

    uint32_t OpenCLManager::maxWGS(uint32_t platformID, uint32_t deviceID)
    {
        std::shared_ptr<cl::Platform> platform = getPlatform(platformID);
        if(platform == nullptr)
        {
            return 0;
        } else
        {
            std::shared_ptr<cl::Device> dev = getDevice(*platform, deviceID);
            if(dev == nullptr)
            {
                return 0;
            } else
            {
                return maxWGS(*dev);
            }
        }
    }

    std::shared_ptr<cl::Device>
    OpenCLManager::getDevice(const cl::Platform& platform, uint32_t deviceID, bool verbose)
    {
        std::vector<cl::Device> all_devices;
        platform.getDevices(CL_DEVICE_TYPE_ALL, &all_devices);
        if(all_devices.size() == 0)
        {
            LOGE << io::xprintf("No devices found for platform %s. Check OpenCL installation!",
                                platform.getInfo<CL_PLATFORM_NAME>().c_str());
            return nullptr;
        }
        if(all_devices.size() <= deviceID)
        {
            LOGE << io::xprintf("There is just %d OpenCL devices on the platform %s and you "
                                "selected invalid zero based index %d.",
                                all_devices.size(), platform.getInfo<CL_PLATFORM_NAME>().c_str(),
                                deviceID);
            return nullptr;
        }
        if(verbose)
        {
            LOGD << io::xprintf("There exists %d OpenCL devices for the platform %s.",
                                all_devices.size(), platform.getInfo<CL_PLATFORM_NAME>().c_str());
            LOGI << io::xprintf("Selected device %d on the platform %s: %s", deviceID,
                                platform.getInfo<CL_PLATFORM_NAME>().c_str(),
                                all_devices[deviceID].getInfo<CL_DEVICE_NAME>().c_str());
        }
        return std::make_shared<cl::Device>(all_devices[deviceID]);
    }

    // Hardcoded platform priority list
    const std::vector<std::string> OpenCLManager::platformPriorityList
        = { "NVIDIA CUDA", "Intel(R) OpenCL", "Intel(R) FPGA Emulation Platform for OpenCL(TM)" };

} // namespace util
} // namespace KCT

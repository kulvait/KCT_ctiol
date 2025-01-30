#include "OPENCL/OpenCLManager.hpp"

namespace KCT {
namespace util {
    uint32_t OpenCLManager::platformCount()
    {
        std::vector<cl::Platform> all_platforms;
        cl::Platform::get(&all_platforms);
        return all_platforms.size();
    }

    std::optional<std::pair<uint32_t, uint32_t>> OpenCLManager::chooseSuitablePlatformAndDevice(bool verbose)
    {
        uint32_t platformsOpenCL = platformCount();
        if (platformsOpenCL == 0)
        {
            LOGE << "No OpenCL platforms found. Check OpenCL installation!";
            return std::nullopt;
        }

        uint32_t CLplatformID = 0;
        uint32_t CLdeviceID = 0;
        uint32_t devicesOnPlatform = 0;
        bool nvidiaPlatformFound = false;

        // First pass: Search for NVIDIA platforms
        for (uint32_t platformID = 0; platformID < platformsOpenCL; platformID++)
        {
            std::string platformName = getPlatformName(platformID);
            if (platformName.find("NVIDIA") != std::string::npos)
            {
                uint32_t currentDevices = deviceCount(platformID);
                if (currentDevices > devicesOnPlatform)
                {
                    devicesOnPlatform = currentDevices;
                    CLplatformID = platformID;
                    CLdeviceID = devicesOnPlatform - 1; // Select the last device
                    nvidiaPlatformFound = true;
                }
            }
        }

        // Second pass: If no NVIDIA platform found, search all platforms
        if (!nvidiaPlatformFound)
        {
            for (uint32_t platformID = 0; platformID < platformsOpenCL; platformID++)
            {
                uint32_t currentDevices = deviceCount(platformID);
                if (currentDevices > devicesOnPlatform)
                {
                    devicesOnPlatform = currentDevices;
                    CLplatformID = platformID;
                    CLdeviceID = devicesOnPlatform - 1; // Select the last device
                }
            }
        }

        if (devicesOnPlatform == 0)
        {
            LOGE << "No devices found on any platform.";
            return std::nullopt;
        }

        if (verbose)
        {
            LOGI << io::xprintf("Selected platform %d: %s", CLplatformID,
                                getPlatformName(CLplatformID).c_str());
            LOGI << io::xprintf("Selected device %d on platform %d.", CLdeviceID, CLplatformID);
        }

        return std::make_pair(CLplatformID, CLdeviceID);
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

} // namespace util
} // namespace KCT

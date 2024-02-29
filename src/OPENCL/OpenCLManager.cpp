#include "OPENCL/OpenCLManager.hpp"

namespace KCT {
namespace util {
    uint32_t OpenCLManager::platformCount()
    {
        std::vector<cl::Platform> all_platforms;
        cl::Platform::get(&all_platforms);
        return all_platforms.size();
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

    cl_ulong OpenCLManager::localMemSize(uint32_t platformID, uint32_t deviceID)
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
                cl_ulong locsize;
                dev->getInfo(CL_DEVICE_LOCAL_MEM_SIZE, &locsize);
                return locsize;
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

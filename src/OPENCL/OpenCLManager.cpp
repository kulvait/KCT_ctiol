#include "OPENCL/OpenCLManager.hpp"

namespace CTL {
namespace util {

    std::shared_ptr<cl::Platform> OpenCLManager::getPlatform(uint32_t id, bool verbose)
    {
        std::vector<cl::Platform> all_platforms;
        cl::Platform::get(&all_platforms);
        if(all_platforms.size() == 0)
        {
            LOGE << "No platforms found. Check OpenCL installation!";
            return nullptr;
        }
        if(all_platforms.size() <= id)
        {
            LOGE << io::xprintf("There is just %d OpenCL platforms on the PC and you selected "
                                "invalid zero based index %d.",
                                all_platforms.size(), id);
            return nullptr;
        }
        if(verbose)
        {
            LOGD << io::xprintf("There exists %d OpenCL platforms on the PC.",
                                all_platforms.size());
            LOGI << io::xprintf("Selected %d-th OpenCL platform: %s.", id,
                                all_platforms[id].getInfo<CL_PLATFORM_NAME>().c_str());
        }
        return std::make_shared<cl::Platform>(all_platforms[id]);
    }

    std::shared_ptr<cl::Device>
    OpenCLManager::getDevice(const cl::Platform& platform, uint32_t id, bool verbose)
    {
        std::vector<cl::Device> all_devices;
        platform.getDevices(CL_DEVICE_TYPE_ALL, &all_devices);
        if(all_devices.size() == 0)
        {
            LOGE << io::xprintf("No devices found for platform %s. Check OpenCL installation!",
                                platform.getInfo<CL_PLATFORM_NAME>().c_str());
            return nullptr;
        }
        if(all_devices.size() <= id)
        {
            LOGE << io::xprintf("There is just %d OpenCL devices on the platform %s and you "
                                "selected invalid zero based index %d.",
                                all_devices.size(), platform.getInfo<CL_PLATFORM_NAME>().c_str(),
                                id);
            return nullptr;
        }
        if(verbose)
        {
            LOGD << io::xprintf("There exists %d OpenCL devices for the platform %s.",
                                all_devices.size(), platform.getInfo<CL_PLATFORM_NAME>().c_str());
            LOGI << io::xprintf("Selected %d-th device on the platform %s: %s", id,
                                platform.getInfo<CL_PLATFORM_NAME>().c_str(),
                                all_devices[id].getInfo<CL_DEVICE_NAME>().c_str());
        }
        return std::make_shared<cl::Device>(all_devices[id]);
    }

} // namespace util
} // namespace CTL

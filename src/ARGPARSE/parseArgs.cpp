#include "ARGPARSE/parseArgs.h"

namespace CTL {
namespace util {

    std::vector<int> processFramesSpecification(std::string frameSpecification, int dimz)
    {
        // Remove spaces
        frameSpecification.erase(std::remove_if(frameSpecification.begin(), frameSpecification.end(), ::isspace), frameSpecification.end());
        frameSpecification = std::regex_replace(frameSpecification, std::regex("end"),
                                                io::xprintf("%d", dimz - 1).c_str());
        LOGD << io::xprintf("Processing frame specification '%s'.", frameSpecification.c_str());
        std::vector<int> frames;
        if(frameSpecification.empty())
        {
            for(int i = 0; i != dimz; i++)
                frames.push_back(i);
        } else
        {
            std::list<std::string> string_list;
            strtk::parse(frameSpecification, ",", string_list);
            auto it = string_list.begin();
            while(it != string_list.end())
            {
                LOGD << io::xprintf("Iteration where the iterator value is '%s'.", (*it).c_str());
                size_t numRangeSigns = std::count(it->begin(), it->end(), '-');
                if(numRangeSigns > 1)
                {
                    std::string msg = io::xprintf(
                        "Wrong number of range specifiers in the string %s.", (*it).c_str());
                    LOGE << msg;
                    throw std::runtime_error(msg);
                } else if(numRangeSigns == 1)
                {
                    std::vector<int> int_vector;
                    strtk::parse((*it), "-", int_vector);
                    if(0 <= int_vector[0] && int_vector[0] <= int_vector[1] && int_vector[1] < dimz)
                    {
                        for(int k = int_vector[0]; k != int_vector[1] + 1; k++)
                        {
                            frames.push_back(k);
                        }
                    } else
                    {
                        std::string msg
                            = io::xprintf("String %s is invalid range specifier.", (*it).c_str());
                        LOGE << msg;
                        throw std::runtime_error(msg);
                    }
                } else
                {
                    int index = std::stoi(it->c_str());
                    if(0 <= index && index < dimz)
                    {
                        frames.push_back(index);
                    } else
                    {
                        std::string msg = io::xprintf(
                            "String %s is invalid specifier for the value in the range [0,%d).",
                            (*it).c_str(), dimz);
                        LOGE << msg;
                        throw std::runtime_error(msg);
                    }
                }
                it++;
            }
        }
        return frames;
    }

} // namespace util
} // namespace CTL

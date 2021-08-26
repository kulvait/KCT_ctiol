#include "PROG/ArgumentsFramespec.hpp" //Command line parser

using namespace KCT;
using namespace KCT::util;

void ArgumentsFramespec::addFramespecArgs()
{

    registerOption("frames",
                   cliApp->add_option(
                       "-f,--frames", frameSpecification,
                       "Frames to process. Frames numbers are zero based. Input can be a range "
                       "i.e. 0-20 or individual coma separated frames i.e. 1,8,9. Order does "
                       "matter and ranges must be from lowest to highest both included. Accepts "
                       "end literal that means the last frame of the processed file."));
    registerOption(
        "reverse_order",
        cliApp->add_flag("-r,--reverse-order", reverseOrder,
                         "Output in the reverse order of input or reverse specified frames."));
    registerOption(
        "each-kth",
        cliApp
            ->add_option(
                "-k,--each-kth", eachKth,
                "Process only each k-th frame specified by k to output. The frames to output "
                "are then 1st specified, 1+kN, N=1...\\infty if such frame exists. Parametter k "
                "must be positive integer. This operation takes place after processing frame "
                "specification by argument --frames and possible reverse.")
            ->check(CLI::Range(1, 65535)));
}

void ArgumentsFramespec::fillFramesVector(uint32_t dimz)
{
    frames.clear();
    if(frameSpecification != "" || reverseOrder || eachKth > 1)
    {
        framesSpecified = true;
    }
    frames = processFramesSpecification(frameSpecification, dimz);
    if(reverseOrder)
    {
        std::reverse(frames.begin(), frames.end()); // It really does!
    }
    if(eachKth > 1)
    {
        std::vector<int> f;
        for(std::size_t i = 0; i != frames.size(); i++)
        {
            if(i % eachKth == 0)
            {
                f.push_back(frames[i]);
            }
        }
        frames = f;
    }
}

std::vector<int> ArgumentsFramespec::processFramesSpecification(std::string frameSpecification,
                                                                int dimz)
{
    // Remove spaces
    frameSpecification.erase(
        std::remove_if(frameSpecification.begin(), frameSpecification.end(), ::isspace),
        frameSpecification.end());
    frameSpecification = std::regex_replace(frameSpecification, std::regex("end"),
                                            io::xprintf("%d", dimz - 1).c_str());
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
            size_t numRangeSigns = std::count(it->begin(), it->end(), '-');
            if(numRangeSigns > 1)
            {
                std::string msg = io::xprintf("Wrong number of range specifiers in the string %s.",
                                              (*it).c_str());
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

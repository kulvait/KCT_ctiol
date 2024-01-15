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
                       "end literal that means the last frame of the processed file. Frames might "
                       "not describe only third dimension but flat index of frame."));
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

void ArgumentsFramespec::fillFramesVector(uint64_t frameCount)
{
    frames.clear();
    if(frameSpecification != "" || reverseOrder || eachKth > 1)
    {
        framesSpecified = true;
    }
    frames = processFramesSpecification(frameSpecification, frameCount);
    if(reverseOrder)
    {
        std::reverse(frames.begin(), frames.end()); // It really does!
    }
    if(eachKth > 1)
    {
        std::vector<uint64_t> f;
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

std::vector<uint64_t> ArgumentsFramespec::processFramesSpecification(std::string frameSpecification,
                                                                     uint64_t frameCount)
{
    std::string ERR;
    // Remove spaces
    frameSpecification.erase(
        std::remove_if(frameSpecification.begin(), frameSpecification.end(), ::isspace),
        frameSpecification.end());
    frameSpecification = std::regex_replace(frameSpecification, std::regex("end"),
                                            io::xprintf("%d", frameCount - 1).c_str());
    std::vector<uint64_t> frames;
    if(frameSpecification.empty())
    {
        for(uint64_t i = 0; i != frameCount; i++)
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
                std::string msg = io::xprintf(
                    "Error parsing framespec: Wrong number of range specifiers in the string %s.",
                    (*it).c_str());
                KCTERR(msg);
            } else if(numRangeSigns == 1)
            {
                std::list<std::string> rangeString;
                strtk::parse((*it), "-", rangeString);
                if(rangeString.size() != 2)
                {
                    ERR = io::xprintf(
                        "Error parsing framespec: Size of rangeString is %d but shall be 2!",
                        rangeString.size());
                    KCTERR(ERR);
                }
                uint64_t from = std::stoull(rangeString.front().c_str());
                uint64_t to = std::stoull(rangeString.back().c_str());
                if(0 <= from && from <= to && to < frameCount)
                {
                    for(uint64_t k = from; k != to + 1; k++)
                    {
                        frames.push_back(k);
                    }
                } else
                {
                    std::string msg = io::xprintf(
                        "Error parsing framespec: String %s is invalid range specifier.",
                        (*it).c_str());
                    KCTERR(msg);
                }
            } else
            {
                uint64_t index = std::stoull(it->c_str());
                if(0 <= index && index < frameCount)
                {
                    frames.push_back(index);
                } else
                {
                    std::string msg = io::xprintf("Error parsing framespec: String %s is invalid "
                                                  "specifier for the value in the range [0,%lu).",
                                                  (*it).c_str(), frameCount);
                    KCTERR(msg);
                }
            }
            it++;
        }
    }
    return frames;
}

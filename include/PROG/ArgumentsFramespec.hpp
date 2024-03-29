#pragma once

#include "CLI/CLI.hpp" //Command line parser
#include "PROG/Arguments.hpp"

namespace KCT::util {
// Using virtual inheritance to include only one copy of Arguments class into the pegigree
class ArgumentsFramespec : public virtual Arguments
{
public:
    ArgumentsFramespec(int argc, char** argv, std::string prgName)
        : Arguments(argc, argv, prgName){};

    std::string frameSpecification = "";
    uint32_t eachKth = 1;
    bool reverseOrder = false;
    bool framesSpecified = false;
    std::vector<uint64_t> frames;
    void fillFramesVector(uint64_t numberOfFrames);

protected:
    void addFramespecArgs();

private:
    static std::vector<uint64_t> processFramesSpecification(std::string frameSpecification, uint64_t frameCount);
};

} // namespace KCT::util

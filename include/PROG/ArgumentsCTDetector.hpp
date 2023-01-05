#pragma once
#include <cctype>
#include <string>

#include "PROG/Arguments.hpp"
#include "rawop.h"

namespace KCT::util {

class ArgumentsCTDetector : public virtual Arguments
{
public:
    // Dimensions
    uint32_t projectionSizeX = 616;
    uint32_t projectionSizeY = 480;
    uint32_t projectionSizeZ = 248;
    // Discretization
    double pixelSizeX = 0.616;
    double pixelSizeY = 0.616;

protected:
    ArgumentsCTDetector(int argc, char* argv[], std::string appName);

    CLI::Option_group* getGeometryGroup();
    void addProjectionSizeArgs();
    void addPixelSizeArgs(double pixelSizeX = 0.616, double PixelSizeY = 0.616);
};
} // namespace KCT::util

#include "PROG/ArgumentsCTDetector.hpp"

namespace KCT::util {

ArgumentsCTDetector::ArgumentsCTDetector(int argc, char** argv, std::string appName)
    : Arguments(argc, argv, appName)
{
}

CLI::Option_group* ArgumentsCTDetector::getGeometryGroup()
{
    CLI::Option_group* og_geometry = getRegisteredOptionGroup("Geometry specification");
    if(og_geometry == nullptr)
    {
        og_geometry = cliApp->add_option_group(
            "Geometry specification", "Specification of the dimensions of the CT geometry.");
        registerOptionGroup("Geometry specification", og_geometry);
    }
    return og_geometry;
}

void ArgumentsCTDetector::addProjectionSizeArgs()
{
    using namespace CLI;
    CLI::Option_group* og_geometry = getGeometryGroup();
    Option* px = og_geometry->add_option(
        "--projection-sizex", projectionSizeX,
        io::xprintf("X dimension of detector in pixel count, defaults to %d.", projectionSizeX));
    Option* py = og_geometry->add_option(
        "--projection-sizey", projectionSizeY,
        io::xprintf("Y dimension of detector in pixel count, defaults to %d.", projectionSizeY));
    px->needs(py);
    py->needs(px);
}

void ArgumentsCTDetector::addPixelSizeArgs(double pixelSizeX, double pixelSizeY)
{
    using namespace CLI;
    this->pixelSizeX = pixelSizeX;
    this->pixelSizeY = pixelSizeY;
    CLI::Option_group* og_geometry = getGeometryGroup();
    Option* psx
        = og_geometry
              ->add_option(
                  "--pixel-sizex", pixelSizeX,
                  io::xprintf("X spacing of detector cells in mm, defaults to %0.3f.", pixelSizeX))
              ->check(CLI::Range(0.0, 10000.00));
    Option* psy
        = og_geometry
              ->add_option(
                  "--pixel-sizey", pixelSizeY,
                  io::xprintf("Y spacing of detector cells in mm, defaults to %0.3f.", pixelSizeY))
              ->check(CLI::Range(0.0, 10000.00));
    psx->needs(psy);
    psy->needs(psx);
    registerOption("--pixel-sizex", psx);
    registerOption("--pixel-sizey", psy);
}

} // namespace KCT::util

#include "PROG/ArgumentsForce.hpp" //Command line parser

using namespace KCT;
using namespace KCT::util;

void ArgumentsForce::addForceArgs()
{

    registerOption(
        "force",
        cliApp->add_flag("--force", force,
                         io::xprintf("Overwrite output files if they exist, defaults to %s.",
                                     force ? "true" : "false")));
}

const int ArgumentsForce::handleFileExistence(const std::string& f, bool force, bool removeIfExist)
{
    std::string ERR;
    if(io::pathExists(f))
    {
        if(force)
        {
            if(removeIfExist)
            {
                LOGI << io::xprintf("Removing existing file %s", f.c_str());
                std::remove(f.c_str());
            } else
            {
                return -1;
            }
        } else
        {
            ERR = io::xprintf(
                "Error: output file %s already exists, use --force to force overwrite.", f.c_str());
            LOGE << ERR;
            return 1;
        }
    }
    return 0;
}

const int ArgumentsForce::handleFileExistence(const std::string& f,
                                              bool force,
                                              KCT::io::DenFileInfo compatible_inf)
{
    std::string ERR;
    if(io::pathExists(f))
    {
        if(force)
        {
            io::DenFileInfo inf(f);
            std::string compatibleF = compatible_inf.getFileName();
            if(!inf.isValid())
            {
                LOGI << io::xprintf("File %s is not valid DEN file, removing!", f.c_str());
                std::remove(f.c_str());
            } else if(inf.getElementType() != compatible_inf.getElementType())
            {
                LOGI << io::xprintf("File %s has incompatible file type with %s, removing!",
                                    f.c_str(), compatibleF.c_str());
                std::remove(f.c_str());
            } else if(inf.dimx() != compatible_inf.dimx() || inf.dimy() != compatible_inf.dimy()
                      || inf.dimz() != compatible_inf.dimz())
            {
                LOGI << io::xprintf("File %s has different dimensions than %s, removing!",
                                    f.c_str(), compatibleF.c_str());
                std::remove(f.c_str());
            } else
            {
                return -1;
            }
        } else
        {
            ERR = io::xprintf(
                "Error: output file %s already exists, use --force to force overwrite.", f.c_str());
            LOGE << ERR;
            return 1;
        }
    }
    return 0;
}

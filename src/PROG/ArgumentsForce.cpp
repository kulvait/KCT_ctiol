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

const int ArgumentsForce::handleFileExistence(const std::string &f, bool force, bool removeIfExist)
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

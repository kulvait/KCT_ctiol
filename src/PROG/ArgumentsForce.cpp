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

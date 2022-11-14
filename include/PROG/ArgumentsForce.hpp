#pragma once

#include "CLI/CLI.hpp" //Command line parser
#include "rawop.h"
#include "PROG/Arguments.hpp"

namespace KCT::util {
// Virtual inheritance to have only one instance of Arguments in the pedigree
class ArgumentsForce : public virtual Arguments
{
public:
    ArgumentsForce(int argc, char** argv, std::string prgName)
        : Arguments(argc, argv, prgName){};

    /**
     * Standard handling of file existence.
     *
     * @param f File name.
     * @param force Forca parametter.
     * @param removeIfExist Remove file f if it exists.
     *
     * @return   0 if file do not exist or it exists and force is specified.
     */
    const int handleFileExistence(const std::string &f, bool force, bool removeIfExist);

    bool force = false;

protected:
    void addForceArgs();
};

} // namespace KCT::util

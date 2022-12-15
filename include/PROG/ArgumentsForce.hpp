#pragma once

#include "CLI/CLI.hpp" //Command line parser
#include "DEN/DenFileInfo.hpp"
#include "PROG/Arguments.hpp"
#include "rawop.h"

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
     * @param force Force parametter.
     * @param removeIfExist Remove file f if it exists.
     *
     * @return 1 if file exists and force is not specified  0 if file do not exist or it was
     * removed, -1 if file exists and is kept.
     */
    const int handleFileExistence(const std::string& f, bool force, bool removeIfExist);

    /**
     * @brief
     *
     * @param f File name.
     * @param force Force parametter.
     * @param compatible_inf DenFileInfo of file that shall be compatible
     *
     * @return 1 if file exists and force is not specified  0 if file do not exist or it was
     * removed, -1 if file exists and is kept.
     */
    const int
    handleFileExistence(const std::string& f, bool force, KCT::io::DenFileInfo compatible_inf);

    bool force = false;

protected:
    void addForceArgs();
};

} // namespace KCT::util

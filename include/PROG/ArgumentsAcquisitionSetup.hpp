#pragma once

#include "CLI/CLI.hpp" //Command line parser
#include "PROG/Arguments.hpp"

namespace CTL::util {
// Using virtual inheritance to include only one copy of Arguments class into the pegigree
class ArgumentsAcquisitionSetup : public virtual Arguments
{
public:
    ArgumentsAcquisitionSetup(int argc, char** argv, std::string prgName)
        : Arguments(argc, argv, prgName){};

    // Acquisition setup parameters
   CLI::Option_group* og_acquisition = nullptr;
    /**
     *Size of pause between sweeps [ms].
     *
     *Computed from DICOM files as 2088.88889ms. Based on experiment, it is 1171ms.
     */
    float pause_size = 1171;
    /** Frame Time. (0018, 1063) Nominal time (in msec) per individual frame.
     *
     *The model assumes that there is delay between two consecutive frames of the frame_time.
     *First frame is aquired directly after pause. From DICOM it is 16.6666667ms. From
     *experiment 16.8ms.
     */
    float frame_time = 16.8;
    uint32_t sweep_count = 10;
	uint32_t angles_per_sweep = 248;

protected:
    void addCarmAcquisitionSetupGroup();
    void addCarmAcquisitionSetupArgs(bool sweepCountAsConfigurableArgument = false, bool anglesPerSweepAsConfigurableArgument = false);

private:
};

} // namespace CTL::util

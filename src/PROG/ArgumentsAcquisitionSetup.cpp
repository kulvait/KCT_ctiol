#include "PROG/ArgumentsAcquisitionSetup.hpp" //Command line parser

using namespace KCT;
using namespace KCT::util;

void ArgumentsAcquisitionSetup::addCarmAcquisitionSetupGroup()
{
    if(og_acquisition == nullptr)
    {
        og_acquisition = cliApp->add_option_group(
            "Acquisition setup",
            "Specification of the C-Arm CT acquisition timings and other parameters.");
        registerOptionGroup("acquisition", og_acquisition);
    }
}

void ArgumentsAcquisitionSetup::addCarmAcquisitionSetupArgs(bool sweepCountAsConfigurableArgument, bool anglesPerSweepAsConfigurableArgument)
{
    addCarmAcquisitionSetupGroup();

    std::string optstr = io::xprintf(
        "Size of pause between two consecutive sweeps [ms]. Defaults to %.1f.", pause_size);
    CLI::Option* option = og_acquisition->add_option("-s,--pause-size", pause_size, optstr);
    registerOption("pause_size", option);

    optstr = io::xprintf("Frame time. Nominal time (in msec) per individual frame, dicom (0018, "
                         "1063) [ms]. Defaults to %.1f.",
                         frame_time);
    option = og_acquisition->add_option("-f,--frame-time", frame_time, optstr);
    registerOption("frame_time", option);

    if(sweepCountAsConfigurableArgument)
    {
        optstr = io::xprintf(
            "Sweep count. Number of full rotations per acquisition. Forward rotation "
            "with consecutive backward rotation are counted as two sweeps [defaults to %d].",
            sweep_count);
        option = og_acquisition->add_option("--sweep-count", sweep_count, optstr);
        registerOption("sweep_count", option);
    }
    if(anglesPerSweepAsConfigurableArgument)
    {
        optstr = io::xprintf("Number of angles per single sweep [default is %d].", angles_per_sweep);

        option = og_acquisition->add_option("--angles-per-sweep", angles_per_sweep, optstr)
                     ->check(CLI::Range(1, 1000));
        registerOption("angles_per_sweep", option);
    }
}

#include "PROG/ArgumentsBasisSpecification.hpp"

using namespace CTL;
using namespace CTL::util;

void ArgumentsBasisSpecification::addBasisSpecificationGroup()
{
    if(og_basis == nullptr)
    {
        og_basis = cliApp->add_option_group("Basis specification",
                                            "Specification of the basis and its parameters.");
        registerOptionGroup("basis", og_basis);
    }
}

void ArgumentsBasisSpecification::addBasisTypeGroup()
{
    addBasisSpecificationGroup();
    if(og_basis_type == nullptr)
    {
        og_basis_type
            = og_basis->add_option_group("Basis type", "Specification of the basis type.");
        registerOptionGroup("basis_type", og_basis_type);
    }
}

void ArgumentsBasisSpecification::addFourierBasisSpecificationGroup()
{
    addBasisSpecificationGroup();
    if(og_fourier_basis == nullptr)
    {
        og_fourier_basis = og_basis->add_option_group(
            "Fourier basis specification",
            "Parameters that apply to the specification of the Fourier basis.");
        registerOptionGroup("fourier_basis", og_fourier_basis);
    }
}

void ArgumentsBasisSpecification::addPolynomialBasisSpecificationGroup()
{
    addBasisSpecificationGroup();
    if(og_polynomial_basis == nullptr)
    {
        og_polynomial_basis = og_basis->add_option_group(
            "Polynomial basis setting", "Select type of polynomials either Legendre or Chebyshev.");
        registerOptionGroup("polynomial_basis", og_polynomial_basis);
    }
}

void ArgumentsBasisSpecification::addEngineerBasisSpecificationGroup()
{
    addBasisSpecificationGroup();
    if(og_engineer_basis == nullptr)
    {
        og_engineer_basis = og_basis->add_option_group("Engineer basis setting",
                                                       "Select fitting type and possible shift.");
        registerOptionGroup("engineer_basis", og_engineer_basis);
    }
}

void ArgumentsBasisSpecification::addOutsideBasisSpecificationGroup()
{
    addBasisSpecificationGroup();
    if(og_outside_basis == nullptr)
    {
        og_outside_basis = og_basis->add_option_group(
            "Outside basis values handling specification",
            "Specify how to handle values outside basis support interval.");
        registerOptionGroup("outside_basis", og_outside_basis);
    }
}

void ArgumentsBasisSpecification::addBasisSpecificationArgs(bool includeBasisSize,
                                                            bool includeBasisSetSelectionArgs)
{
    addBasisSpecificationGroup();
    std::string optstr;
    CLI::Option* option;
    if(includeBasisSetSelectionArgs)
    {
        addBasisTypeGroup();
        addFourierBasisSpecificationArgs();

        option = og_basis_type->add_flag("--legendre", useLegendrePolynomials,
                                         "Use Legendre polynomials.");
        registerOption("legendre", option);

        option
            = og_basis_type->add_flag("--chebyshev", useChebyshevPolynomials, "Use Fourier basis.");
        registerOption("chebyshev", option);

        option = og_basis_type->add_flag("--fourier", useFourierBasis, "Use Fourier basis.");
        registerOption("fourier", option);
        getRegisteredOption("half_periodic_functions")->needs(option);

        option = og_basis_type->add_option("--engineer", engineerBasis,
                                           "Use basis that is stored in a DEN file.");
        registerOption("engineer", option);

        og_basis_type->require_option(1);
    }

    if(includeBasisSize)
    {
        option = og_basis
                     ->add_option("--basis-size", basisSize,
                                  io::xprintf("Size of the basis. Defaults to %d.", basisSize))
                     ->check(CLI::Range(1, 65535));
        registerOption("basis_size", option);
    }
    optstr = io::xprintf(
        "From frame_time and pause_size is computed the scan time and time of "
        "acquisition of particular frames. In reality time dynamics might apply after "
        "some delay from the acquisition of the first frame due to the mask image or "
        "contrast delay. This parameter controls the lenght of the time interval [ms] "
        "from the start of the acquisition to the time when the basis functions are "
        "used to estimate dynamics. Before this time basis functions are considered "
        "having the same value as at the beggining of their support [defaults to %.1f].",
        start_offset);
    option = og_basis->add_option("-i,--start-offset", start_offset, optstr)
                 ->check(CLI::Range(0.0, 100000.0));
    registerOption("start_offset", option);

    optstr = io::xprintf(
        "From frame_time and pause_size is computed the scan time and time of the "
        "acquisition of particular frames. In reality we can assume that time dynamic "
        "does not affect the beginning and the end of the acquisition. This parameter "
        "controls the length of the time interval [ms] before the end of the "
        "acquisition in which all basis functions are considered having the same value "
        "as at the end of their support [defaults to %.1f].",
        end_offset);
    option = og_basis->add_option("-e,--end-offset", end_offset, optstr)
                 ->check(CLI::Range(0.0, 100000.0));
    registerOption("end_offset", option);
    addOutsideBasisSpecificationGroup();
    optstr
        = io::xprintf("In the interval outside [start_offset, acquisition - end_offset] is still "
                      "performed the fitting when the parameter is false, when the parameter is "
                      "true, data outside of this interval is not used at all [defaults to %s].",
                      removeOutsideInterval ? "true" : "false");
    option = og_outside_basis->add_flag("--remove-outside-interval", removeOutsideInterval, optstr);
    registerOption("remove_outside_interval", option);

    optstr = io::xprintf("Data outside interval are fitted on the first function endinterval, "
                         "other functions zero [defaults to %s].",
                         constantOutsideInterval ? "true" : "false");
    option = og_outside_basis->add_flag("--constant-outside-interval", constantOutsideInterval,
                                        optstr);
    registerOption("constant_outside_interval", option);

    optstr = io::xprintf(
        "Extended endvalues of the basis functions outside interval [defaults to %s].",
        endvalueOutsideInterval ? "true" : "false");
    option = og_outside_basis->add_flag("--endvalue-outside-interval", endvalueOutsideInterval,
                                        optstr);
    registerOption("endvalue_outside_interval", option);
    og_outside_basis->require_option(1);
}

void ArgumentsBasisSpecification::addFourierBasisSpecificationArgs()
{
    addFourierBasisSpecificationGroup();
    std::string optstr;
    CLI::Option* option;
    optstr = io::xprintf("Use Fourier basis and include half periodic functions [defaults to");
    if(halfPeriodicFunctions)
    {
        optstr = optstr + " true].";
    } else
    {
        optstr = optstr + " false].";
    }
    option = og_fourier_basis->add_flag("--half-periodic-functions", halfPeriodicFunctions, optstr);
    registerOption("half_periodic_functions", option);
}

void ArgumentsBasisSpecification::addPolynomialBasisSpecificationArgs()
{
    addPolynomialBasisSpecificationGroup();
    og_polynomial_basis->add_flag("--chebyshev", useChebyshevPolynomials,
                                  "Use Chebyshev polynomials.");
    og_polynomial_basis->add_flag("--legendre", useLegendrePolynomials,
                                  "Use Legendre polynomials.");
    og_polynomial_basis->require_option(1);
}

void ArgumentsBasisSpecification::addEngineerBasisSpecificationArgs()
{

    addEngineerBasisSpecificationGroup();
    std::string optstr;
    optstr = "Fit splines instead of step functions [defaults to";
    if(fitSplinesToTheBasisBreakpoints)
    {
        optstr = optstr + " true].";
    } else
    {
        optstr = optstr + " false].";
    }
    og_engineer_basis->add_flag("--spline-fit", fitSplinesToTheBasisBreakpoints, optstr);
    optstr = io::xprintf("Time shift applied to the basis functions relative to the requested time "
                         "of evaluation [defaults to %f].",
                         basisTimeShift);
    og_engineer_basis->add_option("--basis-time-shift", basisTimeShift, optstr);
}

#pragma once

#include "CLI/CLI.hpp" //Command line parser
#include "PROG/Arguments.hpp"

namespace CTL::util {
// Using virtual inheritance to include only one copy of Arguments class into the pegigree
class ArgumentsBasisSpecification : public virtual Arguments
{
public:
    ArgumentsBasisSpecification(int argc, char** argv, std::string prgName)
        : Arguments(argc, argv, prgName){};

    // Basis specification
    CLI::Option_group* og_basis = nullptr;
    CLI::Option_group* og_basis_type = nullptr;
    bool useLegendrePolynomials = false;
    bool useChebyshevPolynomials = false;
    bool useFourierBasis = false;
    std::string engineerBasis = "";
    // Basis and timings
    uint32_t basisSize = 5;
    /** Controls the size of the time interval [ms] that should be identified with 0.0.
     *
     * For offsetted acquisitions also often 4145.
     */
    float start_offset = 0.0, end_offset = 0.0;

    /**How to treat functions outside the interval support
     *
     * removeOutsideInterval ... don't consider values outside interval and delete them from
     * analysis constantOutsideInterval .. consider only the endvalues of the first basis function
     * other zero endvalueOutsideInterval .. extend endvalues of every basis function, this might
     * lead to the loss of orthogonality
     */
    CLI::Option_group* og_outside_basis = nullptr;
    bool removeOutsideInterval = false;
    bool constantOutsideInterval = false;
    bool endvalueOutsideInterval = false;

    CLI::Option_group* og_fourier_basis = nullptr;
    // In the interval outside [start_offset, end_offset] is still performed the fitting when the
    // parameter is false, when the parameter is true, data outside of this interval is not used at
    // all
    bool halfPeriodicFunctions = false;

    CLI::Option_group* og_polynomial_basis = nullptr;
	
	CLI::Option_group* og_engineer_basis = nullptr;
	bool fitSplinesToTheBasisBreakpoints = false;
	float basisTimeShift = 0.0;
	

protected:
    void addBasisSpecificationGroup();
    void addOutsideBasisSpecificationGroup();
    void addBasisTypeGroup();
    void addBasisSpecificationArgs(bool includeBasisSize, bool includeBasisSetSelectionArgs);
    void addFourierBasisSpecificationGroup();
    void addFourierBasisSpecificationArgs();
    void addPolynomialBasisSpecificationGroup();
    void addPolynomialBasisSpecificationArgs();
    void addEngineerBasisSpecificationGroup();
    void addEngineerBasisSpecificationArgs();

private:
};

} // namespace CTL::util

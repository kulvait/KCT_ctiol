#pragma once
// External libraries

// Internal libraries
//#include "MATRIX/ProjectionMatrix.hpp"
#include "DEN/DenFileInfo.hpp"
#include "ProjectionMatrixReaderI.hpp"

namespace CTL {
namespace io {
    /**
     *Interface for reading Projection matrices objects.
     *
     *The implementation might be "slim implementation" that access underlying source each time it
     *is called. The implementation might be also "fat implementation" that holds in memory
     *structure that source each call.
     */
    class DenProjectionMatrixReader : ProjectionMatrixReaderI
    {
    private:
        int countMatrices;
        std::string projectionMatrixFile;

    public:
        DenProjectionMatrixReader(std::string projectionMatrixFile);

        /*Returns i-th projection slice in the source.*/
        matrix::ProjectionMatrix readMatrix(int i) override;
        /**Number of projection matrices in the source.*/
        unsigned int count() const override;
    };
} // namespace io
} // namespace CTL

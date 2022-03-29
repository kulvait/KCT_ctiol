#pragma once
// External libraries

// Internal libraries
#include "MATRIX/ProjectionMatrix.hpp"

namespace KCT {
namespace io {
    /**
     *Interface for reading projection matrices.
     *
     *The implementation might be "slim implementation" that access underlying source each time it
     *is called. The implementation might be also "fat implementation" that holds in memory
     *structure that source each call.
     */
    class ProjectionMatrixReaderI
    {
    public:
        /*Returns i-th projection slice in the source.*/
        virtual matrix::ProjectionMatrix readMatrix(int i) = 0;
        /**Number of projection matrices in the source.*/
        virtual uint32_t count() const = 0;
        virtual ~ProjectionMatrixReaderI() = default;
    };
} // namespace io
} // namespace KCT

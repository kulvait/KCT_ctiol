#pragma once
// External libraries

// Internal libraries
#include "Frame2DI.hpp"
//#include "matrix.h"
#include "MATRIX/ProjectionMatrix.hpp"

namespace KCT {
namespace io {
    /**
     *Interface for reading projections and related projection matrices in the format specified in
     *"Matrix-Template" project.
     *
     *The implementation might be "slim implementation" that access underlying source each time it
     *is called. The implementation might be also "fat implementation" that holds in memory
     *structure that source each call.
     */
    template <typename T>
    class ProjectionReaderI
    {
    public:
        virtual std::shared_ptr<matrix::ProjectionMatrix> readProjectionMatrix(int i) = 0;
        /**Returns the i-th projection matrix from the source.
         */
        virtual std::shared_ptr<io::Frame2DI<T>> readProjectionSlice(int i) = 0;
        /*Returns i-th projection slice in the source.*/
        virtual unsigned int dimx() const = 0;
        /**Returns x dimension.*/
        virtual unsigned int dimy() const = 0;
        /**Returns y dimension.*/
        virtual unsigned int dimz() const = 0;
        /**Returns number of slices in the source, slices are indexed 0 <= i < count().*/
    };
} // namespace io
} // namespace KCT

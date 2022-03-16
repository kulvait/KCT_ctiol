#pragma once

// External libraries
#include "itkImage.h"
// Internal libraries

namespace KCT {
namespace io {
    /**
    Interface for writing images. It is not necessery to write matrices along them.
    */
    template <typename T>
    class AsyncFrame2DWritterItkI
    {
    public:
        /**Writes i-th slice to the source.*/
        virtual void writeFrame(typename itk::Image<T, 2>::Pointer s, int i) = 0;

        /** @return x dimension*/
        virtual unsigned int dimx() const = 0;

        /** @return y dimension*/
        virtual unsigned int dimy() const = 0;

        /** @return z dimension*/
        virtual unsigned int dimz() const = 0;
    };
} // namespace io
} // namespace KCT

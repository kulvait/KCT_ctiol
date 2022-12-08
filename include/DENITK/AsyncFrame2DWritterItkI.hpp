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
        virtual void writeFrame(typename itk::Image<T, 2>::Pointer s, uint32_t i) = 0;

        /** @return x dimension*/
        virtual uint32_t dimx() const = 0;

        /** @return y dimension*/
        virtual uint32_t dimy() const = 0;

        /** @return z dimension*/
        virtual uint32_t dimz() const = 0;

        /** virtual destructor*/
        virtual ~AsyncFrame2DWritterItkI() = default;
    };
} // namespace io
} // namespace KCT

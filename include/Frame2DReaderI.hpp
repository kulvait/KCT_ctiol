#pragma once
// External libraries

// Internal libraries
#include "Frame2DI.hpp"

namespace CTL {
namespace io {
    /**
     *Interface for reading Frame2D objects.
     *
     *The implementation might be "slim implementation" that access underlying source each time it
     *is called. The implementation might be also "fat implementation" that holds in memory
     *structure that source each call.
     */
    template <typename T>
    class Frame2DReaderI
    {
    public:
        virtual std::shared_ptr<io::Frame2DI<T>> readFrame(unsigned int i) = 0;
        /*Returns i-th projection slice in the source.*/
        virtual unsigned int dimx() const = 0;
        /**Returns x dimension.*/
        virtual unsigned int dimy() const = 0;
        /**Returns y dimension.*/
        virtual unsigned int dimz() const = 0;
        /**Returns number of slices in the source, slices are indexed 0 <= i < count().*/
    };
} // namespace io
} // namespace CTL

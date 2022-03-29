#pragma once

// Internal libraries
#include "Frame2DI.hpp"

namespace KCT {
namespace io {
    /**
    Interface for writing images. It is not necessery to write matrices along them.
    */
    template <typename T>
    class AsyncFrame2DWritterI
    {
    public:
        /**Writes i-th slice to the source.*/
        virtual void writeFrame(const Frame2DI<T>& s, uint32_t i) = 0;

        /**Returns x dimension.*/
        virtual uint32_t dimx() const = 0;

        /**Returns y dimension.*/
        virtual uint32_t dimy() const = 0;

        /**Returns z dimension.*/
        virtual uint32_t dimz() const = 0;

        virtual ~AsyncFrame2DWritterI() = default;
    };
} // namespace io
} // namespace KCT

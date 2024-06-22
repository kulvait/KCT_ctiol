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
        virtual void writeFrame(const Frame2DI<T>& s, uint64_t k) = 0;

        /**Returns x dimension.*/
        virtual uint32_t dimx() const = 0;

        /**Returns y dimension.*/
        virtual uint32_t dimy() const = 0;

        /**Number of frames.*/
        virtual uint64_t getFrameCount() const = 0;

        /**Frame size.*/
        virtual uint64_t getFrameSize() const = 0;

        /**Frame byte size.*/
        virtual uint64_t getFrameByteSize() const = 0;

        virtual ~AsyncFrame2DWritterI() = default;
    };
} // namespace io
} // namespace KCT

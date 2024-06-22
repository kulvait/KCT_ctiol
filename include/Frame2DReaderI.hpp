#pragma once
// External libraries

// Internal libraries
#include "Frame2DI.hpp"

namespace KCT {
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
        /*Returns k-th frame in the source.*/
        virtual std::shared_ptr<io::Frame2DI<T>> readFrame(uint64_t k) = 0;

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

        virtual ~Frame2DReaderI() = default;
    };
} // namespace io
} // namespace KCT

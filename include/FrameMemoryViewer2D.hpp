#pragma once

#include "DEN/DenSupportedType.hpp"
#include "Frame2DI.hpp"

namespace KCT {
namespace io {
    /** Provide view of the memory in the form of Frame2DI. Do not manage the memory.
     *
     */
    template <typename T>
    class FrameMemoryViewer2D : public Frame2DI<T>
    {
    public:
        FrameMemoryViewer2D(T* frameDataPointer, uint32_t sizex, uint32_t sizey)
            : frameDataPointer(frameDataPointer)
            , sizex(sizex)
            , sizey(sizey)
        {
            this->frameSize = (uint64_t)sizex * (uint64_t)sizey;
        }

        FrameMemoryViewer2D(const FrameMemoryViewer2D& b)
            : FrameMemoryViewer2D(b.frameDataPointer, b.sizex, b.sizey)
        {
        } // copy constructor

        ~FrameMemoryViewer2D() {} // destructor
        // copy assignment
        FrameMemoryViewer2D& operator=(const FrameMemoryViewer2D& b) = delete;

        // Move constructor
        FrameMemoryViewer2D(FrameMemoryViewer2D&& other) = delete;

        // Move assignment
        FrameMemoryViewer2D& operator=(FrameMemoryViewer2D&& other) = delete;

        /**Get the value at coordinates x, y.*/
        T get(uint32_t x, uint32_t y) const override { return frameDataPointer[y * sizex + x]; }

        /**Set the value at coordinates x, y.*/
        void set(T val, uint32_t x, uint32_t y) override { frameDataPointer[y * sizex + x] = val; }

        /**Returns x dimension.*/
        uint32_t dimx() const override { return sizex; }

        /**Returns y dimension.*/
        uint32_t dimy() const override { return sizey; }

        /**Returns frameSize.*/
        uint64_t getFrameSize() const override { return frameSize; }

    private:
        T* frameDataPointer;
        uint32_t sizex, sizey;
        uint64_t frameSize;
    };
} // namespace io
} // namespace KCT

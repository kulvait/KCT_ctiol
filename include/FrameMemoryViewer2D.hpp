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
        FrameMemoryViewer2D(T* slice, int sizex, int sizey)
        {
            this->slice = slice;
            this->sizex = sizex;
            this->sizey = sizey;
        }

        FrameMemoryViewer2D(const FrameMemoryViewer2D& b)
            : FrameMemoryViewer2D(b.slice, b.sizex, b.sizey)
        {
        } // copy constructor

        ~FrameMemoryViewer2D() {} // destructor

        /**Get the value at coordinates x, y.*/
        T get(unsigned int x, unsigned int y) const override { return slice[y * sizex + x]; }

        /**Set the value at coordinates x, y.*/
        void set(T val, unsigned int x, unsigned int y) override { slice[y * sizex + x] = val; }

        /**Returns x dimension.*/
        unsigned int dimx() const override { return sizex; }

        /**Returns y dimension.*/
        unsigned int dimy() const override { return sizey; }

    private:
        T* slice;
        int sizex, sizey;
    };
} // namespace io
} // namespace KCT

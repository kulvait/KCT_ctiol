#pragma once

#include "io/DenSupportedType.hpp"

namespace CTL {
namespace io {
    /** Interface to access one two dimensional slice of the multidimensional source data
     *
     *Intention is to read the subarray of the three dimensional array T A[x, y, z], where one
     *(typically z) dimendion is fixed. T is a type of the data used, might be float, double or
     *uint16_t.
     */
    template <typename T>
    class Frame2DI
    {
    public:
        /**Get the value at coordinates x, y.*/
        virtual T get(unsigned int x, unsigned int y) const = 0;

        /**Get the value at coordinates x, y. Calls get(x,y).*/
        T operator()(unsigned int x, unsigned int y) const { return get(x, y); }

        /**Set the value at coordinates x, y.*/
        virtual void set(T val, unsigned int x, unsigned int y) = 0;

        /**Returns x dimension.*/
        virtual unsigned int dimx() const = 0;

        /**Returns y dimension.*/
        virtual unsigned int dimy() const = 0;
    };
} // namespace io
} // namespace CTL

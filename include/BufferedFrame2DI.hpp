#pragma once

//Internal
#include "Frame2DI.hpp"

namespace KCT::io {
/** Interface to access one two dimensional frame of a three dimensional array. This is used to read the data from the file in a buffered way.
 *
 *Intention is to read the subarray of the three dimensional array T A[x, y, z], where one
 *(typically z) dimendion is fixed. T is a type of the data used, might be float, double or
 *uint16_t.
 */
template <typename T>
class BufferedFrame2DI : public Frame2DI<T>
{
public:
    virtual T* data() = 0;
    virtual const T* data() const = 0;
    virtual ~BufferedFrame2DI() override = default;
};
} // namespace KCT::io

#pragma once

#include "io/Frame2DI.hpp"

namespace CTL {
namespace io {
    /// Interface to read one two dimensional slice of the multidimensional source data
    /**
     *Intention is to read the subarray of the three dimensional array T A[x, y, z], where one
     *(typically z) dimendion is fixed. T is a type of the data used, might be float, double or
     *uint16_t.
     */
    template <typename T>
    class ItkImageChunk : public Frame2DI<T>
    {
        typename itk::Image<T, 2>::Pointer img;

    public:
        ItkImageChunk(typename itk::Image<T, 2>::Pointer img)
            : img(img)
        {
        }

        T get(unsigned int x, unsigned int y) const override
        {
            typename itk::Image<T, 2>::IndexType i = { { x, y } };
            typename itk::Image<T, 2>::PixelType p = img->GetPixel(i);
            return p;
        }
        /**Get the value at coordinates x, y.*/

        T operator()(unsigned int x, unsigned int y) const { return get(x, y); }
        /**Get the value at coordinates x, y. Calls get(x,y).*/

        unsigned int dimx() const override
        {
            typename itk::Image<T, 2>::RegionType region = img->GetLargestPossibleRegion();
            typename itk::Image<T, 2>::SizeType size = region.GetSize();
            return size[0];
        }

        unsigned int dimy() const override
        {
            typename itk::Image<T, 2>::RegionType region = img->GetLargestPossibleRegion();
            typename itk::Image<T, 2>::SizeType size = region.GetSize();
            return size[1];
        }
    };
} // namespace io
} // namespace CTL

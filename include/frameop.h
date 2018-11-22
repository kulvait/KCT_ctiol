#pragma once
// Basic operation on the structure Frame2DI. These functions are not logically so close to the
// structure to be its member functions. Min, max, median...

#include <plog/Log.h>

#include <cmath>

#include "DEN/DenSupportedType.hpp"
#include "Frame2DI.hpp"

namespace CTL {
namespace io {

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wreturn-type"
    /**Minimal value.
     *
     *Excluding any NaN values if they are present.
     *
     */
    template <typename T>
    T minFrameValue(const Frame2DI<T>& f)
    {
        DenSupportedType dataType = io::getSupportedTypeByByteLength(sizeof(T));
        int dimx = f.dimx();
        int dimy = f.dimy();
        switch(dataType)
        {
        case io::DenSupportedType::uint16_t_:
        {
            uint16_t min = 65535;
            uint16_t a;
            for(int i = 0; i != dimx; i++)
                for(int j = 0; j != dimy; j++)
                {
                    a = f.get(i, j);
                    min = (a < min ? a : min);
                }
            return min;
        }
        case io::DenSupportedType::float_:
        {
            float min = std::numeric_limits<float>::quiet_NaN(); // Comparing to NAN results false
                                                                 // according to IEEE standard
            float a;
            for(int i = 0; i != dimx; i++)
                for(int j = 0; j != dimy; j++)
                {
                    a = f.get(i, j);
                    if(!std::isnan(a))
                    {
                        min = (a > min ? min : a);
                    }
                }
            return min;
        }
        case io::DenSupportedType::double_:
        {
            double min = std::numeric_limits<double>::quiet_NaN(); // Comparing to NAN results false
                                                                   // according to IEEE standard
            double a;
            for(int i = 0; i != dimx; i++)
                for(int j = 0; j != dimy; j++)
                {
                    a = f.get(i, j);
                    if(!std::isnan(a))
                    {
                        min = (a > min ? min : a);
                    }
                }
            return min;
        }
        default:
            io::throwerr("Unsupported type %s!", io::DenSupportedTypeToString(dataType));
            // This function actually throws an error.
        }
    }

    /**Maximal value.
     *
     *Excluding any NaN values if they are present.
     *
     */
    template <typename T>
    T maxFrameValue(const Frame2DI<T>& f)
    {
        DenSupportedType dataType = io::getSupportedTypeByByteLength(sizeof(T));
        int dimx = f.dimx();
        int dimy = f.dimy();
        switch(dataType)
        {
        case io::DenSupportedType::uint16_t_:
        {
            uint16_t max = 0;
            uint16_t a;
            for(int i = 0; i != dimx; i++)
                for(int j = 0; j != dimy; j++)
                {
                    a = f.get(i, j);
                    max = (a > max ? a : max);
                }
            return max;
        }
        case io::DenSupportedType::float_:
        {
            float max = std::numeric_limits<float>::quiet_NaN(); // Comparing to NAN results false
                                                                 // according to IEEE standard
            float a;
            for(int i = 0; i != dimx; i++)
                for(int j = 0; j != dimy; j++)
                {
                    a = f.get(i, j);
                    if(!std::isnan(a))
                    {
                        max = (a < max ? max : a);
                    }
                }
            return max;
        }
        case io::DenSupportedType::double_:
        {
            double max = std::numeric_limits<double>::quiet_NaN(); // Comparing to NAN results false
                                                                   // according to IEEE standard
            double a;
            for(int i = 0; i != dimx; i++)
                for(int j = 0; j != dimy; j++)
                {
                    a = f.get(i, j);
                    if(!std::isnan(a))
                    {
                        max = (a < max ? max : a);
                    }
                }
            return max;
        }
        default:
            io::throwerr("Unsupported type %s!", io::DenSupportedTypeToString(dataType));
            // This function actually throws an error.
        }
    }

#pragma GCC diagnostic pop

    /**Median value.
     *
     *Excluding any NaN values if they are present. Returns the value at the position dimx*dimy/2 in
     *the sorted array.
     *
     */
    template <typename T>
    T medianFrameValue(const Frame2DI<T>& f)
    {
        DenSupportedType dataType = io::getSupportedTypeByByteLength(sizeof(T));
        int dimx = f.dimx();
        int dimy = f.dimy();
        std::vector<T> vec;
        vec.reserve(dimx * dimy);
        switch(dataType)
        {
        case io::DenSupportedType::uint16_t_:
        {
            for(int i = 0; i != dimx; i++)
                for(int j = 0; j != dimy; j++)
                {
                    vec.push_back(f.get(i, j));
                }
            break;
        }
        case io::DenSupportedType::float_:
        case io::DenSupportedType::double_:
        {
            for(int i = 0; i != dimx; i++)
                for(int j = 0; j != dimy; j++)
                {
                    if(!std::isnan(f.get(i, j)))
                    {
                        vec.push_back(f.get(i, j));
                    }
                }
            break;
        }
        default:
            io::throwerr("Unsupported type %s!", io::DenSupportedTypeToString(dataType));
        }

        std::sort(vec.begin(), vec.end());
        return vec[(dimx * dimy) / 2];
    }

    /**L_normExponent norm.
     *
     *If there are NaN or infinite values, the result will be NaN.
     *For other values computes L_normExponent norm.
     *
     */
    template <typename T>
    double normFrame(const Frame2DI<T>& f, int normExponent = 2)
    {
        if(normExponent < 1)
        {
            io::throwerr("Lx norm is computed for positive integers but the exponent is %d.",
                         normExponent);
        }

        if(sumNonfiniteValues(f) > 0)
            return std::numeric_limits<double>::quiet_NaN();
        int dimx = f.dimx();
        int dimy = f.dimy();
        T a;
        T sum = 0;

        for(int i = 0; i != dimx; i++)
            for(int j = 0; j != dimy; j++)
            {
                a = (double)f.get(i, j);
                sum += std::pow(std::abs(a), (double)normExponent);
            }
        return std::pow(sum, 1.0 / (double)normExponent);
    }

    /**Mean value.
     *
     *Excluding any NaN values if they are present.
     *
     */
    template <typename T>
    double meanFrameValue(const Frame2DI<T>& f)
    {
        DenSupportedType dataType = io::getSupportedTypeByByteLength(sizeof(T));
        int dimx = f.dimx();
        int dimy = f.dimy();
        switch(dataType)
        {
        case io::DenSupportedType::uint16_t_:
        {
            double sum = 0;
            for(int i = 0; i != dimx; i++)
                for(int j = 0; j != dimy; j++)
                {
                    sum += (double)f.get(i, j);
                }
            return sum / (dimx * dimy);
        }
        case io::DenSupportedType::float_:
        case io::DenSupportedType::double_:
        {
            double sum = 0;
            int nonNanCount = 0;
            for(int i = 0; i != dimx; i++)
                for(int j = 0; j != dimy; j++)
                {
                    if(!std::isnan(f.get(i, j)))
                    {
                        sum += (double)f.get(i, j);
                        nonNanCount++;
                    }
                }
            return sum / nonNanCount;
        }
        }
    }

    /**Number of NaN values  in the Frame2DI.
     *
     *For uint16 is always 0.
     *If the type can be NaN, this is number of items for which std::isnan(f.get(i, j)).
     *
     */
    template <typename T>
    int sumNanValues(const Frame2DI<T>& f)
    {
        DenSupportedType dataType = io::getSupportedTypeByByteLength(sizeof(T));
        switch(dataType)
        {
        case io::DenSupportedType::uint16_t_:
            return 0;
        case io::DenSupportedType::float_:
        case io::DenSupportedType::double_:
            int nv = 0;
            int dimx = f.dimx();
            int dimy = f.dimy();
            for(int i = 0; i != dimx; i++)
            {
                for(int j = 0; j != dimy; j++)
                {
                    if(std::isnan(f.get(i, j)))
                    {
                        nv++;
                    }
                }
            }
            return nv;
        }
    }

    /**Number of finite values in the Frame2DI.
     *
     *For uint16 is always 0.
     *For floats or doubles it calls isfinite on every element. Val returned is umber of nonfinites.
     *From std::isfinite reference: Determines if the given floating point number arg has finite
     *value i.e. it is normal, subnormal or zero, but not infinite or NaN.
     *
     */
    template <typename T>
    int sumNonfiniteValues(const Frame2DI<T>& f)
    {
        DenSupportedType dataType = io::getSupportedTypeByByteLength(sizeof(T));
        switch(dataType)
        {
        case io::DenSupportedType::uint16_t_:
            return 0;
        case io::DenSupportedType::float_:
        case io::DenSupportedType::double_:
            int nfc = 0;
            int dimx = f.dimx();
            int dimy = f.dimy();
            for(int i = 0; i != dimx; i++)
            {
                for(int j = 0; j != dimy; j++)
                {
                    if(!std::isfinite(f.get(i, j)))
                    {
                        nfc++;
                    }
                }
            }
            return nfc;
        }
    }
} // namespace io
} // namespace CTL

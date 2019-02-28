#pragma once
// Logging
#include <plog/Log.h>

// Standard libraries
#include <cmath>
#include <string>

// Internal libraries
#include "DEN/DenNextElement.h"
#include "DEN/DenSupportedType.hpp"
#include "rawop.h" //To get number of rows...

namespace CTL {
namespace io {

    class DenFileInfo
    {
    public:
        DenFileInfo(std::string fileName);
        uint16_t dimx() const;
        uint16_t dimy() const;
        uint16_t dimz() const;
        uint16_t getNumRows() const;
        uint16_t getNumCols() const;
        uint16_t getNumSlices() const;
        uint64_t getSize() const;
        uint64_t getNumPixels() const;
        DenSupportedType getDataType() const;
        uint8_t elementByteSize() const;
        template <typename T>
        T getMaxVal() const;
        template <typename T>
        T getMinVal() const;
        template <typename T>
        T getl2Square() const;

    private:
        std::string fileName;
    };

    template <typename T>
    T DenFileInfo::getMaxVal() const
    {
        DenSupportedType dataType = getDataType();
        uint64_t rows = getNumRows();
        uint64_t cols = getNumCols();
        uint64_t zdim = getNumSlices();
        uint64_t currentPosition;
        uint64_t offset = 6;
        switch(dataType)
        {
        case io::DenSupportedType::uint16_t_:
        {
            uint16_t max = 0;
            uint8_t* buffer = new uint8_t[rows * cols * 2];
            for(uint64_t z = 0; z != zdim; z++)
            {
                currentPosition = offset + z * rows * cols * 2;
                io::readBytesFrom(fileName, currentPosition, buffer, rows * cols * 2);
                T val;
                for(uint64_t pos = 0; pos != rows * cols; pos++)
                {
                    val = util::getNextElement<T>(&buffer[pos * 2], dataType);
                    max = (max > val ? max : val);
                }
            }
            delete[] buffer;
            return max;
        }
        case io::DenSupportedType::float_:
        {
            float max = -std::numeric_limits<float>::infinity();
            uint8_t* buffer = new uint8_t[rows * cols * 4];
            for(uint64_t z = 0; z != zdim; z++)
            {
                currentPosition = offset + z * rows * cols * 4;
                io::readBytesFrom(fileName, currentPosition, buffer, rows * cols * 4);
                T val;
                for(uint64_t pos = 0; pos != rows * cols; pos++)
                {
                    val = util::getNextElement<T>(&buffer[pos * 4], dataType);
                    if(!std::isnan(val))
                    {
                        max = (max > val ? max : val);
                    }
                }
            }
            delete[] buffer;
            return max;
        }
        case io::DenSupportedType::double_:
        {
            double max = -std::numeric_limits<double>::infinity();
            uint8_t* buffer = new uint8_t[rows * cols * 8];
            for(uint64_t z = 0; z != zdim; z++)
            {
                currentPosition = offset + z * rows * cols * 8;
                io::readBytesFrom(fileName, currentPosition, buffer, rows * cols * 8);
                T val;
                for(uint64_t pos = 0; pos != rows * cols; pos++)
                {
                    val = util::getNextElement<T>(&buffer[pos * 8], dataType);
                    if(!std::isnan(val))
                    {
                        max = (max > val ? max : val);
                    }
                }
            }
            delete[] buffer;
            return max;
        }
        default:
            std::string errMsg
                = io::xprintf("Unsupported data type %s.", io::DenSupportedTypeToString(dataType));
            LOGE << errMsg;
            throw std::runtime_error(errMsg);
        }
    }

    template <typename T>
    T DenFileInfo::getMinVal() const
    {
        DenSupportedType dataType = getDataType();
        uint64_t rows = getNumRows();
        uint64_t cols = getNumCols();
        uint64_t zdim = getNumSlices();
        uint64_t currentPosition;
        uint64_t offset = 6;
        switch(dataType)
        {
        case io::DenSupportedType::uint16_t_:
        {
            uint16_t min = 65535;
            uint8_t* buffer = new uint8_t[rows * cols * 2];
            for(uint64_t z = 0; z != zdim; z++)
            {
                currentPosition = offset + z * rows * cols * 2;
                io::readBytesFrom(fileName, currentPosition, buffer, rows * cols * 2);
                T val;
                for(uint64_t pos = 0; pos != rows * cols; pos++)
                {
                    val = util::getNextElement<T>(&buffer[pos * 2], dataType);
                    min = (min < val ? min : val);
                }
            }
            delete[] buffer;
            return min;
        }
        case io::DenSupportedType::float_:
        {
            float min = std::numeric_limits<float>::infinity();
            uint8_t* buffer = new uint8_t[rows * cols * 4];
            for(uint64_t z = 0; z != zdim; z++)
            {
                currentPosition = offset + z * rows * cols * 4;
                io::readBytesFrom(fileName, currentPosition, buffer, rows * cols * 4);
                T val;
                for(uint64_t pos = 0; pos != rows * cols; pos++)
                {
                    val = util::getNextElement<T>(&buffer[pos * 4], dataType);
                    if(!std::isnan(val))
                    {
                        min = (min < val ? min : val);
                    }
                }
            }
            delete[] buffer;
            return min;
        }
        case io::DenSupportedType::double_:
        {
            double min = std::numeric_limits<double>::infinity();
            uint8_t* buffer = new uint8_t[rows * cols * 8]; // This is problematic should be new
            for(uint64_t z = 0; z != zdim; z++)
            {

                currentPosition = offset + z * rows * cols * 8;
                io::readBytesFrom(fileName, currentPosition, buffer, rows * cols * 8);
                T val;
                for(uint64_t pos = 0; pos != rows * cols; pos++)
                {
                    val = util::getNextElement<T>(&buffer[pos * 8], dataType);
                    if(!std::isnan(val))
                    {
                        min = (min < val ? min : val);
                    }
                }
            }
            delete[] buffer;
            return min;
        }
        default:
            std::string errMsg
                = io::xprintf("Unsupported data type %s.", io::DenSupportedTypeToString(dataType));
            LOGE << errMsg;
            throw std::runtime_error(errMsg);
        }
    }

    /**
     * Get square of l2 norm for given file.
     *
     * @return |x|_2^2
     */
    template <typename T>
    T DenFileInfo::getl2Square() const
    {
        DenSupportedType dataType = getDataType();
        uint64_t dim_x = dimx();
        uint64_t dim_y = dimy();
        uint64_t dim_z = dimz();
        uint64_t currentPosition;
        uint64_t offset = 6;
        double sum = 0.0;
        double val;
        switch(dataType)
        {
        case io::DenSupportedType::uint16_t_:
        {
            uint8_t* buffer = new uint8_t[dim_x * dim_y * 2];
            for(uint64_t z = 0; z != dim_z; z++)
            {
                currentPosition = offset + z * dim_x * dim_y * 2;
                io::readBytesFrom(fileName, currentPosition, buffer, dim_x * dim_y * 2);
                for(uint64_t pos = 0; pos != dim_y * dim_x; pos++)
                {
                    val = (double)util::getNextElement<T>(&buffer[pos * 2], dataType);
                    sum += val * val;
                }
            }
            delete[] buffer;
            return sum;
        }
        case io::DenSupportedType::float_:
        {
            uint8_t* buffer = new uint8_t[dim_y * dim_x * 4];
            for(uint64_t z = 0; z != dim_z; z++)
            {
                currentPosition = offset + z * dim_y * dim_x * 4;
                io::readBytesFrom(fileName, currentPosition, buffer, dim_y * dim_x * 4);
                for(uint64_t pos = 0; pos != dim_y * dim_x; pos++)
                {
                    val = (double)util::getNextElement<T>(&buffer[pos * 4], dataType);
                    sum += val * val;
                }
            }
            delete[] buffer;
            return sum;
        }
        case io::DenSupportedType::double_:
        {
            uint8_t* buffer = new uint8_t[dim_y * dim_x * 8]; // This is problematic should be new
            for(uint64_t z = 0; z != dim_z; z++)
            {

                currentPosition = offset + z * dim_y * dim_x * 8;
                io::readBytesFrom(fileName, currentPosition, buffer, dim_y * dim_x * 8);
                for(uint64_t pos = 0; pos != dim_y * dim_x; pos++)
                {
                    val = (double)util::getNextElement<T>(&buffer[pos * 8], dataType);
                    sum += val * val;
                }
            }
            delete[] buffer;
            return sum;
        }
        default:
            std::string errMsg
                = io::xprintf("Unsupported data type %s.", io::DenSupportedTypeToString(dataType));
            LOGE << errMsg;
            throw std::runtime_error(errMsg);
        }
    }

} // namespace io
} // namespace CTL

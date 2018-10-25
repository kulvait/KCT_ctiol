#pragma once
// Logging
#include <plog/Log.h>

// Standard libraries
#include <cmath>
#include <string>

// Internal libraries
#include "DEN/DenSupportedType.hpp"
#include "rawop.h" //To get number of rows...
#include "DEN/DenNextElement.h"

namespace CTL {
namespace io {

    class DenFileInfo
    {
    public:
        DenFileInfo(std::string fileName);
        int getNumRows();
        int getNumCols();
        int getNumSlices();
        long getSize();
        long getNumPixels();
        DenSupportedType getDataType();
        int elementByteSize();
        template <typename T>
        T getMaxVal();
        template <typename T>
        T getMinVal();

    private:
        std::string fileName;
    };

    template <typename T>
    T DenFileInfo::getMaxVal()
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
            for(int z = 0; z != zdim; z++)
            {
                currentPosition = offset + z * rows * cols * 2;
                io::readBytesFrom(fileName, currentPosition, buffer, rows * cols * 2);
                T val;
                for(int pos = 0; pos != rows * cols; pos++)
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
            for(int z = 0; z != zdim; z++)
            {
                currentPosition = offset + z * rows * cols * 4;
                io::readBytesFrom(fileName, currentPosition, buffer, rows * cols * 4);
                T val;
                for(int pos = 0; pos != rows * cols; pos++)
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
            for(int z = 0; z != zdim; z++)
            {
                currentPosition = offset + z * rows * cols * 8;
                io::readBytesFrom(fileName, currentPosition, buffer, rows * cols * 8);
                T val;
                for(int pos = 0; pos != rows * cols; pos++)
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
    T DenFileInfo::getMinVal()
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
            for(int z = 0; z != zdim; z++)
            {
                currentPosition = offset + z * rows * cols * 2;
                io::readBytesFrom(fileName, currentPosition, buffer, rows * cols * 2);
                T val;
                for(int pos = 0; pos != rows * cols; pos++)
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
            for(int z = 0; z != zdim; z++)
            {
                currentPosition = offset + z * rows * cols * 4;
                io::readBytesFrom(fileName, currentPosition, buffer, rows * cols * 4);
                T val;
                for(int pos = 0; pos != rows * cols; pos++)
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
            for(int z = 0; z != zdim; z++)
            {

                currentPosition = offset + z * rows * cols * 8;
                io::readBytesFrom(fileName, currentPosition, buffer, rows * cols * 8);
                T val;
                for(int pos = 0; pos != rows * cols; pos++)
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

} // namespace io
} // namespace CTL

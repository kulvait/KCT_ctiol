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

namespace KCT::io {

class DenFileInfo
{
public:
    DenFileInfo(std::string fileName);
    std::string getFileName() const;
    uint32_t dimx() const;
    uint32_t dimy() const;
    uint32_t dimz() const;
    uint64_t dimflatz() const;
    uint64_t frameSize() const;
    /**
     * Get n-th dimension
     * @param n Dimension index x=0, y=1, z=2, ....
     *
     * @return
     */
    uint16_t dimCount() const;
    uint32_t dim(uint32_t n) const;
    uint32_t getNumRows() const;
    uint32_t getNumCols() const;
    uint32_t getNumSlices() const;
    /**
     * Get file size.
     *
     * @return Size of the underlying file in bytes.
     */
    uint64_t getSize() const;
    uint64_t elementCount() const;
    bool isExtended() const;
    // If true the array is sorted such that (x,y,z) = (0,0,0), (1,0,0), (0,1,0), (1,1,0),
    // (0,0,1), (1,0,1), (0,1,1), (1,1,1)  If false the array is sorted such that (x,y,z) =
    // (0,0,0), (0,1,0), (1,0,0), (1,1,0), (0,0,1), (0,1,1), (1,0,1) (1,1,1)
    bool hasXMajorAlignment() const;
    bool isValid() const;
    uint64_t getOffset() const;
    DenSupportedType getDataType() const;
    uint16_t elementByteSize() const;
    template <typename T>
    T getMaxVal() const;
    template <typename T>
    T getMinVal() const;
    template <typename T>
    double getl2Square() const;
    template <typename T>
    double getMean() const;
    template <typename T>
    double getVariance() const;
    template <typename T>
    void readFlatFrameIntoBuffer(uint64_t flatZIndex,
                                 T* bufferToFill,
                                 bool fillXMajor,
                                 uint8_t* tmpbuffer) const;
    template <typename T>
    void readIntoBuffer(T* buffer, bool xmajor = true);
    static void
    createLegacyDenHeader(std::string fileName, uint16_t dimx, uint16_t dimy, uint16_t dimz);
    static void create3DDenHeader(std::string fileName,
                                  DenSupportedType dst,
                                  uint32_t dimx,
                                  uint32_t dimy,
                                  uint32_t dimz,
                                  bool XMajorAlignment = true);

    static void createDenHeader(std::string fileName,
                                DenSupportedType dst,
                                uint16_t dimCount,
                                uint32_t* dim,
                                bool XMajorAlignment = true);
    static void createEmptyLegacyDenFile(
        std::string fileName, DenSupportedType dst, uint16_t dimx, uint16_t dimy, uint16_t dimz);
    static void createEmpty3DDenFile(std::string fileName,
                                     DenSupportedType dst,
                                     uint32_t dimx,
                                     uint32_t dimy,
                                     uint32_t dimz,
                                     bool XMajorAlignment = true);

    static void createEmptyDenFile(std::string fileName,
                                   DenSupportedType dst,
                                   uint16_t dimCount,
                                   uint32_t* dim,
                                   bool XMajorAlignment = true);

private:
    std::string fileName;
    // Extended format specification is at https://kulvait.github.io/KCT_doc/den-format.html
    uint16_t _dimCount;
    uint16_t _elementByteSize;
    DenSupportedType _elementType;
    std::array<double, 16> _dim;
    uint64_t _fileSize;
    bool extended = false;
    bool XMajorAlignment = true;
    uint64_t offset = 6;
};

template <typename T>
T DenFileInfo::getMaxVal() const
{
    DenSupportedType dataType = getDataType();
    uint64_t rows = getNumRows();
    uint64_t cols = getNumCols();
    uint64_t zdim = getNumSlices();
    uint64_t currentPosition;
    switch(dataType)
    {
    case io::DenSupportedType::UINT16: {
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
    case io::DenSupportedType::FLOAT32: {
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
    case io::DenSupportedType::FLOAT64: {
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
        std::string errMsg = io::xprintf("Unsupported data type %s.",
                                         io::DenSupportedTypeToString(dataType).c_str());
        KCTERR(errMsg);
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
    switch(dataType)
    {
    case io::DenSupportedType::UINT16: {
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
    case io::DenSupportedType::FLOAT32: {
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
    case io::DenSupportedType::FLOAT64: {
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
        std::string errMsg = io::xprintf("Unsupported data type %s.",
                                         io::DenSupportedTypeToString(dataType).c_str());
        KCTERR(errMsg);
    }
}

/**
 * Get square of l2 norm for given file.
 *
 * @return |x|_2^2
 */
template <typename T>
double DenFileInfo::getl2Square() const
{
    DenSupportedType dataType = getDataType();
    uint64_t dim_x = dimx();
    uint64_t dim_y = dimy();
    uint64_t dim_z = dimz();
    uint64_t currentPosition;
    double sum = 0.0;
    double val;
    switch(dataType)
    {
    case io::DenSupportedType::UINT16: {
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
    case io::DenSupportedType::FLOAT32: {
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
    case io::DenSupportedType::FLOAT64: {
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
        std::string errMsg = io::xprintf("Unsupported data type %s.",
                                         io::DenSupportedTypeToString(dataType).c_str());
        KCTERR(errMsg);
    }
}

template <typename T>
double DenFileInfo::getMean() const
{
    DenSupportedType dataType = getDataType();
    uint64_t dim_x = dimx();
    uint64_t dim_y = dimy();
    uint64_t dim_z = dimz();
    uint64_t totalSize = dim_x * dim_y * dim_z;
    uint64_t currentPosition;
    uint32_t elementSize = elementByteSize();
    double sum = 0.0;
    double val;
    uint8_t* buffer = new uint8_t[dim_x * dim_y * elementByteSize()];
    for(uint64_t z = 0; z != dim_z; z++)
    {
        currentPosition = offset + z * dim_x * dim_y * elementSize;
        io::readBytesFrom(fileName, currentPosition, buffer, dim_x * dim_y * elementSize);
        for(uint64_t pos = 0; pos != dim_y * dim_x; pos++)
        {
            val = (double)util::getNextElement<T>(&buffer[pos * elementSize], dataType);
            sum += val;
        }
    }
    delete[] buffer;
    return T(sum / totalSize);
}

template <typename T>
double DenFileInfo::getVariance() const
{
    DenSupportedType dataType = getDataType();
    uint32_t elementSize = elementByteSize();
    uint64_t dim_x = dimx();
    uint64_t dim_y = dimy();
    uint64_t dim_z = dimz();
    uint64_t totalSize = dim_x * dim_y * dim_z;
    uint64_t currentPosition;
    double sum = 0.0;
    double val;
    double mean = (double)getMean<T>();
    uint8_t* buffer = new uint8_t[dim_x * dim_y * elementByteSize()];
    for(uint64_t z = 0; z != dim_z; z++)
    {
        currentPosition = offset + z * dim_x * dim_y * elementSize;
        io::readBytesFrom(fileName, currentPosition, buffer, dim_x * dim_y * 2);
        for(uint64_t pos = 0; pos != dim_y * dim_x; pos++)
        {
            val = (double)util::getNextElement<T>(&buffer[pos * elementSize], dataType);
            sum += (val - mean) * (val - mean);
        }
    }
    delete[] buffer;
    return T(sum / totalSize);
}

template <typename T>
void DenFileInfo::readFlatFrameIntoBuffer(uint64_t flatZIndex,
                                          T* bufferToFill,
                                          bool fillXMajor,
                                          uint8_t* tmpbuffer) const
{
    uint64_t _frameSize = this->frameSize();
    uint64_t frameByteSize = _frameSize * _elementByteSize;
    uint64_t position = this->offset + flatZIndex * frameByteSize;
    io::readBytesFrom(this->fileName, position, tmpbuffer, frameByteSize);
    if(fillXMajor == this->XMajorAlignment)
    {
        for(uint64_t a = 0; a != _frameSize; a++)
        {
            bufferToFill[a]
                = util::getNextElement<T>(&tmpbuffer[a * _elementByteSize], _elementType);
        }
    } else
    {
        uint64_t innerIndex, outerIndex;
        uint64_t _dimx = dimx();
        uint64_t _dimy = dimy();
        for(uint64_t x = 0; x != _dimx; x++)
        {
            for(uint64_t y = 0; y != _dimy; y++)
            {
                if(this->XMajorAlignment)
                {
                    innerIndex = x + _dimx * y;
                } else
                {
                    innerIndex = y + _dimy * x;
                }
                if(fillXMajor)
                {
                    outerIndex = x + _dimx * y;
                } else
                {
                    outerIndex = y + _dimy * x;
                }
                bufferToFill[outerIndex] = util::getNextElement<T>(
                    &tmpbuffer[innerIndex * _elementByteSize], _elementType);
            }
        }
    }
}

template <typename T>
void DenFileInfo::readIntoBuffer(T* buffer, bool xmajor)
{
    if(getDenSupportedTypeByTypeID(typeid(T)) != _elementType)
    {
        KCTERR(io::xprintf("Buffer of incompatible type, need to fill buffer of %s.",
                           DenSupportedTypeToString(_elementType).c_str()))
    }

    uint64_t _dimflatz = dimflatz();
    uint64_t _framesize = this->frameSize();
    uint8_t* tmpbuffer = new uint8_t[_framesize * _elementByteSize];
    for(uint64_t k = 0; k != _dimflatz; k++)
    {
        this->readFlatFrameIntoBuffer(k, buffer + k * _framesize, xmajor, tmpbuffer);
    }
    delete[] tmpbuffer;
}

} // namespace KCT::io

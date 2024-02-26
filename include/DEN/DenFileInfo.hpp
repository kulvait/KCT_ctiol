#pragma once
// Logging
#include <plog/Log.h>

// Standard libraries
#include <array>
#include <cmath>
#include <string>

// Internal libraries
#include "DEN/DenNextElement.h"
#include "DEN/DenSupportedType.hpp"
#include "littleEndianAlignment.h"
#include "rawop.h" //To get number of rows...

namespace KCT::io {

class DenFileInfo
{
public:
    DenFileInfo(std::string fileName, bool exceptInvalid = true);
    uint32_t dimx() const;
    uint32_t dimy() const;
    /**
     * @return Flat z dimension, frameCount if it can be represented by uint32_t.
     */
    uint32_t dimz() const;
    uint64_t getFrameCount() const;
    uint64_t getFrameSize() const;
    uint64_t getFrameByteSize() const;
    /**
     * Get n-th dimension
     * @param n Dimension index x=0, y=1, z=2, ....
     *
     * @return
     */
    uint16_t getDimCount() const;
    uint32_t dim(uint32_t n) const;
    uint32_t getNumRows() const;
    uint32_t getNumCols() const;
    uint32_t getNumSlices() const;
    std::string getFileName() const;
    /**
     * Get file size.
     *
     * @return Size of the underlying file in bytes.
     */
    uint64_t getFileSize() const;
    uint64_t getElementCount() const;
    bool isExtended() const;
    // If true the array is sorted such that (x,y,z) = (0,0,0), (1,0,0), (0,1,0), (1,1,0),
    // (0,0,1), (1,0,1), (0,1,1), (1,1,1)  If false the array is sorted such that (x,y,z) =
    // (0,0,0), (0,1,0), (1,0,0), (1,1,0), (0,0,1), (0,1,1), (1,0,1) (1,1,1)
    bool hasXMajorAlignment() const;
    bool isValid();
    uint64_t getOffset() const;
    DenSupportedType getElementType() const;
    uint16_t getElementByteSize() const;
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
    /**
     * Given buffer will be writtern to the given frame indexed by flat z index.
     *
     * @param flatZIndex Flat z index of the frame.
     * @param bufferToWrite c style array, must be compatible with den type, unchecked
     * @param bufferXMajor if the alignment of the buffer is xmajor
     * @param tmpbuffer buffer of the size framesize*sizeof(t)
     */
    void writeBufferIntoFlatFrame(uint64_t flatZIndex,
                                  T* bufferToWrite,
                                  bool bufferXMajor,
                                  uint8_t* tmpbuffer) const;

    /**
     * Flat frame with given z index will be read to the buffer.
     *
     * @param flatZIndex Flat z index of the frame.
     * @param bufferToWrite c style array, must be compatible with den type, unchecked
     * @param bufferXMajor if the alignment of the buffer will be xmajor
     * @param tmpbuffer buffer of the size framesize*sizeof(t)
     */
    template <typename T>
    void readFlatFrameIntoBuffer(uint64_t flatZIndex,
                                 T* bufferToFill,
                                 bool fillXMajor,
                                 uint8_t* tmpbuffer,
                                 uint32_t x_from = 0,
                                 uint32_t x_count = 0,
                                 uint32_t y_from = 0,
                                 uint32_t y_count = 0) const;
    /**
     * Read underlying DEN file into the c_array.
     *
     * @param c_array Array to read file into.
     * @param c_array_xmajor Xmajor alignment of the frames in resulting array, defaults to true.
     * @param x_from specify x range
     * @param x_count specify x range, 0 means to read up to dimx
     * @param y_from specify y range
     * @param y_count specify y range, 0 means to read up to dimy
     */
    template <typename T>
    void readIntoArray(T* c_array,
                       bool c_array_xmajor = true,
                       uint32_t x_from = 0,
                       uint32_t x_count = 0,
                       uint32_t y_from = 0,
                       uint32_t y_count = 0,
                       uint32_t z_from = 0,
                       uint32_t z_count = 0) const;

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

    template <typename T>
    static void create3DDenFileFromArray(T* c_array,
                                         bool c_array_xmajor,
                                         std::string fileName,
                                         DenSupportedType dst,
                                         uint32_t dimx,
                                         uint32_t dimy,
                                         uint32_t dimz,
                                         bool XMajorAlignment = true);

    template <typename T>
    static void createDenFileFromArray(T* c_array,
                                       bool c_array_xmajor,
                                       std::string fileName,
                                       DenSupportedType dst,
                                       uint16_t dimCount,
                                       uint32_t* dim,
                                       bool XMajorAlignment = true);

private:
    bool valid = true;
    std::string fileName;
    uint64_t fileSize;
    // Extended format specification is at https://kulvait.github.io/KCT_doc/den-format.html
    uint16_t dimCount;
    std::array<uint32_t, 16> _dim;
    bool extended = false;
    bool XMajorAlignment = true;
    uint64_t offset = 6;
    DenSupportedType elementType;

    // Information that can be derived from previous data
    uint16_t elementByteSize;
    uint64_t frameSize;
    uint64_t frameByteSize;
    uint64_t elementCount;
    uint64_t frameCount;

    // If x can represent given dimension
    bool isAdmissibleDimension(uint32_t x, uint32_t dimID, bool canBeOfDimSize = false) const;
};

template <typename T>
T DenFileInfo::getMaxVal() const
{
    std::shared_ptr<std::ifstream> ifstream = std::make_shared<std::ifstream>();
    ifstream->open(fileName,
                   std::ios::binary | std::ios::in); // Open binary, for output, for input
    uint64_t currentPosition;
    T max = std::numeric_limits<T>::lowest();
    uint8_t* buffer = new uint8_t[frameByteSize];
    for(uint64_t k = 0; k != frameCount; k++)
    {
        currentPosition = offset + k * frameByteSize;
        io::readBytesFrom(ifstream, currentPosition, buffer, frameByteSize);
        T val;
        for(uint64_t pos = 0; pos != frameSize; pos++)
        {
            val = util::getNextElement<T>(&buffer[pos * elementByteSize], elementType);
            if(val > max)
            {
                max = val;
            }
        }
    }
    delete[] buffer;
    if(ifstream != nullptr)
    {
        ifstream->close();
    }
    return max;
}

template <typename T>
T DenFileInfo::getMinVal() const
{
    std::shared_ptr<std::ifstream> ifstream = std::make_shared<std::ifstream>();
    ifstream->open(fileName,
                   std::ios::binary | std::ios::out
                       | std::ios::in); // Open binary, for output, for input
    uint64_t currentPosition;
    T min = std::numeric_limits<T>::max();
    uint8_t* buffer = new uint8_t[frameByteSize];
    for(uint64_t k = 0; k != frameCount; k++)
    {
        currentPosition = offset + k * frameByteSize;
        io::readBytesFrom(ifstream, currentPosition, buffer, frameByteSize);
        T val;
        for(uint64_t pos = 0; pos != frameSize; pos++)
        {
            val = util::getNextElement<T>(&buffer[pos * elementByteSize], elementType);
            if(val < min)
            {
                min = val;
            }
        }
    }
    delete[] buffer;
    if(ifstream != nullptr)
    {
        ifstream->close();
    }
    return min;
}

/**
 * Get square of l2 norm for given file.
 *
 * @return |x|_2^2
 */
template <typename T>
double DenFileInfo::getl2Square() const
{
    std::shared_ptr<std::ifstream> ifstream = std::make_shared<std::ifstream>();
    ifstream->open(fileName,
                   std::ios::binary | std::ios::out
                       | std::ios::in); // Open binary, for output, for input
    uint64_t currentPosition;
    uint8_t* buffer = new uint8_t[frameByteSize];
    double sumSquares = 0.0;
    for(uint64_t k = 0; k != frameCount; k++)
    {
        currentPosition = offset + k * frameByteSize;
        io::readBytesFrom(ifstream, currentPosition, buffer, frameByteSize);
        double val;
        for(uint64_t pos = 0; pos != frameSize; pos++)
        {
            val = (double)util::getNextElement<T>(&buffer[pos * elementByteSize], elementType);
            sumSquares += (val * val);
        }
    }
    delete[] buffer;
    if(ifstream != nullptr)
    {
        ifstream->close();
    }
    return sumSquares;
}

template <typename T>
double DenFileInfo::getMean() const
{
    uint64_t currentPosition;
    double sum = 0.0;
    double val;
    uint8_t* buffer = new uint8_t[frameByteSize];
    for(uint64_t z = 0; z != frameCount; z++)
    {
        currentPosition = offset + z * frameByteSize;
        io::readBytesFrom(fileName, currentPosition, buffer, frameByteSize);
        for(uint64_t pos = 0; pos != frameSize; pos++)
        {
            val = (double)util::getNextElement<T>(&buffer[pos * elementByteSize], elementType);
            sum += val;
        }
    }
    delete[] buffer;
    return T(sum / elementCount);
}

template <typename T>
double DenFileInfo::getVariance() const
{
    uint64_t currentPosition;
    double sum = 0.0;
    double val;
    double mean = (double)getMean<T>();
    uint8_t* buffer = new uint8_t[frameByteSize];
    for(uint64_t z = 0; z != frameCount; z++)
    {
        currentPosition = offset + z * frameByteSize;
        io::readBytesFrom(fileName, currentPosition, buffer, frameByteSize);
        for(uint64_t pos = 0; pos != frameSize; pos++)
        {
            val = (double)util::getNextElement<T>(&buffer[pos * elementByteSize], elementType);
            sum += (val - mean) * (val - mean);
        }
    }
    delete[] buffer;
    return T(sum / elementCount);
}

template <typename T>
void DenFileInfo::writeBufferIntoFlatFrame(uint64_t flatZIndex,
                                           T* bufferToWrite,
                                           bool bufferXMajor,
                                           uint8_t* tmpbuffer) const
{
    uint64_t position = this->offset + flatZIndex * frameByteSize;
    if(bufferXMajor == this->XMajorAlignment)
    {
        for(uint64_t a = 0; a != frameSize; a++)
        {
            util::setNextElement<T>(bufferToWrite[a], &tmpbuffer[a * elementByteSize]);
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
                if(bufferXMajor)
                {
                    outerIndex = x + _dimx * y;
                } else
                {
                    outerIndex = y + _dimy * x;
                }
                util::setNextElement<T>(bufferToWrite[outerIndex],
                                        &tmpbuffer[innerIndex * elementByteSize]);
            }
        }
    }
    io::writeBytesFrom(this->fileName, position, tmpbuffer, frameByteSize);
}

template <typename T>
void DenFileInfo::readFlatFrameIntoBuffer(uint64_t flatZIndex,
                                          T* bufferToFill,
                                          bool fillXMajor,
                                          uint8_t* tmpbuffer,
                                          uint32_t x_from,
                                          uint32_t x_count,
                                          uint32_t y_from,
                                          uint32_t y_count) const
{
    std::string ERR;
    if(getDenSupportedTypeByTypeID(typeid(T)) != elementType)
    {
        ERR = io::xprintf("File %s has incompatible type, need to fill buffer of %s.",
                          fileName.c_str(), DenSupportedTypeToString(elementType).c_str());
        KCTERR(ERR);
    }
    if(dimCount < 2)
    {
        ERR = io::xprintf("File %s shall have at least 2 dimensions but has %d.", fileName.c_str(),
                          dimCount);
        KCTERR(ERR);
    }
    uint64_t position = this->offset + flatZIndex * frameByteSize;
    io::readBytesFrom(this->fileName, position, tmpbuffer, frameByteSize);
    if(fillXMajor == this->XMajorAlignment && x_from == 0 && (x_count == 0 || x_count == _dim[0])
       && y_from == 0 && (y_count == 0 || y_count == _dim[1]))
    {
        for(uint64_t a = 0; a != frameSize; a++)
        {
            bufferToFill[a] = util::getNextElement<T>(&tmpbuffer[a * elementByteSize], elementType);
        }
    } else
    {
        bool admissibleDimensions = true;
        admissibleDimensions &= isAdmissibleDimension(x_from, 0);
        admissibleDimensions &= isAdmissibleDimension(y_from, 1);
        if(x_count == 0)
        {
            x_count = _dim[0] - x_from;
        }
        if(y_count == 0)
        {
            y_count = _dim[1] - y_from;
        }
        admissibleDimensions &= isAdmissibleDimension(x_from + x_count, 0, true);
        admissibleDimensions &= isAdmissibleDimension(y_from + y_count, 1, true);
        if(!admissibleDimensions)
        {
            ERR = io::xprintf("File %s wrong dimensions specified.", fileName.c_str());
            KCTERR(ERR);
        }
        uint64_t innerIndex, outerIndex;
        uint64_t _dimx = _dim[0];
        uint64_t _dimy = _dim[1];
        uint64_t x_to = x_from + x_count;
        uint64_t y_to = y_from + y_count;
        int i, j;
        for(uint64_t x = x_from; x != x_to; x++)
        {
            i = x - x_from;
            for(uint64_t y = y_from; y != y_to; y++)
            {
                j = y - y_from;
                if(this->XMajorAlignment)
                {
                    innerIndex = x + _dimx * y;
                } else
                {
                    innerIndex = y + _dimy * x;
                }
                if(fillXMajor)
                {
                    outerIndex = i + x_count * j;
                } else
                {
                    outerIndex = j + y_count * i;
                }
                bufferToFill[outerIndex] = util::getNextElement<T>(
                    &tmpbuffer[innerIndex * elementByteSize], elementType);
            }
        }
    }
}

template <typename T>
void DenFileInfo::readIntoArray(T* c_array,
                                bool c_array_xmajor,
                                uint32_t x_from,
                                uint32_t x_count,
                                uint32_t y_from,
                                uint32_t y_count,
                                uint32_t z_from,
                                uint32_t z_count) const
{
    if(dimCount < 3)
    {
        std::string ERR = io::xprintf("File %s shall have at least 3 dimensions but has %d.",
                                      fileName.c_str(), dimCount);
        KCTERR(ERR);
    }
    bool admissibleDimensions = true;
    admissibleDimensions &= isAdmissibleDimension(x_from, 0);
    admissibleDimensions &= isAdmissibleDimension(y_from, 1);
    admissibleDimensions &= isAdmissibleDimension(z_from, 2);
    if(x_count == 0)
    {
        x_count = _dim[0] - x_from;
    }
    if(y_count == 0)
    {
        y_count = _dim[1] - y_from;
    }
    if(z_count == 0)
    {
        z_count = _dim[2] - z_from;
    }
    admissibleDimensions &= isAdmissibleDimension(x_from + x_count, 0, true);
    admissibleDimensions &= isAdmissibleDimension(y_from + y_count, 1, true);
    admissibleDimensions &= isAdmissibleDimension(z_from + z_count, 2, true);
    if(!admissibleDimensions)
    {
        std::string ERR = io::xprintf("File %s wrong dimensions specified.", fileName.c_str());
        KCTERR(ERR);
    }
    uint8_t* tmpbuffer = new uint8_t[frameByteSize];
    uint64_t IND;
    uint64_t arrayFrameSize = static_cast<uint64_t>(x_count) * static_cast<uint64_t>(y_count);
    for(uint64_t k = z_from; k != z_count; k++)
    {
        IND = k - z_from;
        this->readFlatFrameIntoBuffer(k, c_array + IND * arrayFrameSize, c_array_xmajor, tmpbuffer,
                                      x_from, x_count, y_from, y_count);
    }
    delete[] tmpbuffer;
}
template <typename T>
void DenFileInfo::create3DDenFileFromArray(T* c_array,
                                           bool c_array_xmajor,
                                           std::string fileName,
                                           DenSupportedType dst,
                                           uint32_t dimx,
                                           uint32_t dimy,
                                           uint32_t dimz,
                                           bool XMajorAlignment)
{
    std::array<uint32_t, 3> dim = { dimx, dimy, dimz };
    createDenFileFromArray<T>(c_array, c_array_xmajor, fileName, dst, 3, std::begin(dim),
                              XMajorAlignment);
}

template <typename T>
void DenFileInfo::createDenFileFromArray(T* c_array,
                                         bool c_array_xmajor,
                                         std::string fileName,
                                         DenSupportedType dst,
                                         uint16_t dimCount,
                                         uint32_t* dim,
                                         bool XMajorAlignment)
{
    if(getDenSupportedTypeByTypeID(typeid(T)) != dst)
    {
        KCTERR(io::xprintf("Buffer of incompatible type, need to fill buffer of %s.",
                           DenSupportedTypeToString(dst).c_str()))
    }
    if(dim == nullptr)
    {
        KCTERR("Null pointer exception!");
    }
    createEmptyDenFile(fileName, dst, dimCount, dim, XMajorAlignment);
    DenFileInfo fileInfo(fileName);
    uint64_t frameByteSize = fileInfo.getFrameByteSize();
    uint64_t frameCount = fileInfo.getFrameCount();
    uint64_t frameSize = fileInfo.getFrameSize();
    uint8_t* tmpbuffer = new uint8_t[frameByteSize];
    for(uint64_t k = 0; k != frameCount; k++)
    {
        fileInfo.writeBufferIntoFlatFrame(k, c_array + k * frameSize, c_array_xmajor, tmpbuffer);
    }
    delete[] tmpbuffer;
}

} // namespace KCT::io

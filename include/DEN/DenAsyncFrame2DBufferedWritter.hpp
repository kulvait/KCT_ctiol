#pragma once

// External libraries
#include <mutex>
#include <string>
#include <typeinfo>

// Internal libraries
#include "AsyncFrame2DWritterI.hpp"
#include "BufferedFrame2D.hpp"
#include "DEN/DenFileInfo.hpp"
#include "littleEndianAlignment.h"
#include "rawop.h"

namespace KCT::io {
/**
Interface for writing images. It is not necessery to write matrices along them.
*/
template <typename T>
class DenAsyncFrame2DBufferedWritter : public AsyncFrame2DWritterI<T>
{
private:
    std::string denFile;
    uint32_t sizex, sizey, sizez;
    uint64_t frameSize;
    uint64_t frameByteSize;
    uint64_t offset;
    bool extended;
    bool XMajor;
    uint8_t* buffer;
    mutable std::mutex writingMutex;
    std::shared_ptr<std::ofstream> ofstream;
    std::vector<char> _pubsetbuf_buffer;
    uint64_t _pubsetbuf_buffer_size = 1073741824; // 2^30
    bool littleEndianArchitecture;
    void initialize();

public:
    /**
     *	Constructor using file name and sizeensions.
     *	If file exists and have a same sizeensions, not overwrite but if it has different
     *  dimensions, overwrites.
     *
     * @param denFile
     * @param sizex
     * @param sizey
     * @param sizez
     */
    DenAsyncFrame2DBufferedWritter(
        std::string denFile, uint32_t sizex, uint32_t sizey, uint32_t sizez, bool XMajor = true);

    /**
     * Constructor using file name of existing DEN file. It does not imediatelly overwrite or
     * zero the file.
     *
     * @param denFile
     */
    DenAsyncFrame2DBufferedWritter(std::string denFile);

    void writeBufferedFrame(BufferedFrame2D<T>& f, uint32_t k);

    /**Writes i-th frame to the file.*/
    void writeFrame(const Frame2DI<T>& s, uint32_t k) override;

    /**Returns x dimension.*/
    virtual uint32_t dimx() const override;

    /**Returns y dimension.*/
    virtual uint32_t dimy() const override;

    /**Returns z dimension.*/
    virtual uint32_t dimz() const override;

    /**Returns file name.**/
    std::string getFileName() const;

    /// Destructor
    ~DenAsyncFrame2DBufferedWritter();
    /// Copy constructor
    DenAsyncFrame2DBufferedWritter(const DenAsyncFrame2DBufferedWritter<T>& b) = delete;
    // Copy assignment
    DenAsyncFrame2DBufferedWritter<T>& operator=(const DenAsyncFrame2DBufferedWritter<T>& b)
        = delete;
    // Move constructor
    DenAsyncFrame2DBufferedWritter(DenAsyncFrame2DBufferedWritter<T>&& b) = delete;
    // Move assignment
    DenAsyncFrame2DBufferedWritter<T>& operator=(DenAsyncFrame2DBufferedWritter<T>&& other)
        = delete;
};

template <typename T>
void DenAsyncFrame2DBufferedWritter<T>::initialize()
{
    int num = 1;
    littleEndianArchitecture = (*(char*)&num == 1);
    buffer = new uint8_t[frameByteSize];
    ofstream = std::make_shared<std::ofstream>();
    _pubsetbuf_buffer.resize(_pubsetbuf_buffer_size);
    ofstream->rdbuf()->pubsetbuf(&_pubsetbuf_buffer[0], _pubsetbuf_buffer_size);
    ofstream->open(denFile,
                   std::ios::binary | std::ios::out
                       | std::ios::in); // Open binary, for output, for input
}

template <typename T>
DenAsyncFrame2DBufferedWritter<T>::DenAsyncFrame2DBufferedWritter(
    std::string denFile, uint32_t dimx, uint32_t dimy, uint32_t dimz, bool XMajor)
    : denFile(denFile)
    , sizex(dimx)
    , sizey(dimy)
    , sizez(dimz)
    , XMajor(XMajor)
{
    DenSupportedType type = getDenSupportedTypeByTypeID(typeid(T));
    offset = 4096;
    if(io::pathExists(denFile))
    {
        DenFileInfo inf(denFile, false);
        if(inf.isValid() && inf.getDimCount() == 3 && inf.getElementType() == type
           && inf.hasXMajorAlignment() == XMajor && inf.dimx() == dimx && inf.dimy() == dimy
           && inf.dimz() == dimz)
        {
            LOGD << io::xprintf("Will be writting to existing file %s.", denFile.c_str());
            offset = inf.getOffset();
        } else
        {
            DenFileInfo::createEmpty3DDenFile(denFile, type, dimx, dimy, dimz, XMajor);
            LOGD << io::xprintf("Just overwritten the file %s with empty file.", denFile.c_str());
        }
    } else
    {
        DenFileInfo::createEmpty3DDenFile(denFile, type, dimx, dimy, dimz, XMajor);
    }
    frameSize = (uint64_t)sizex * (uint64_t)sizey;
    frameByteSize = sizeof(T) * frameSize;
    initialize();
}

template <typename T>
DenAsyncFrame2DBufferedWritter<T>::DenAsyncFrame2DBufferedWritter(std::string denFile)
{
    std::string err;
    if(!io::pathExists(denFile))
    {
        err = io::xprintf("The file %s does not exist.", denFile.c_str());
        KCTERR(err);
    }
    io::DenFileInfo info(denFile);
    if(!info.isValid())
    {
        err = io::xprintf("The file %s is not valid DEN.", denFile.c_str());
        KCTERR(err);
    }
    uint64_t elementByteSize = sizeof(T);
    if(info.getElementByteSize() != elementByteSize)
    {
        err = io::xprintf("Element byte size %d of %s is incompatible with the size %d of "
                          "current template type.",
                          info.getElementByteSize(), denFile.c_str(), elementByteSize);
        KCTERR(err);
    }
    extended = info.isExtended();
    offset = info.getOffset();
    this->denFile = denFile;
    this->sizex = info.dimx();
    this->sizey = info.dimy();
    this->sizez = info.dimz();
    frameSize = (uint64_t)sizex * (uint64_t)sizey;
    frameByteSize = sizeof(T) * frameSize;
    initialize();
}

/// Guard for move constructor stealed objects
template <typename T>
DenAsyncFrame2DBufferedWritter<T>::~DenAsyncFrame2DBufferedWritter()
{
    if(buffer != nullptr)
        delete[] buffer;
    buffer = nullptr;
    if(ofstream != nullptr)
    {
        ofstream->close();
    }
}

template <typename T>
std::string DenAsyncFrame2DBufferedWritter<T>::getFileName() const
{
    return this->denFile;
}

template <typename T>
uint32_t DenAsyncFrame2DBufferedWritter<T>::dimx() const
{
    return sizex;
}

template <typename T>
uint32_t DenAsyncFrame2DBufferedWritter<T>::dimy() const
{
    return sizey;
}

template <typename T>
uint32_t DenAsyncFrame2DBufferedWritter<T>::dimz() const
{
    return sizez;
}

template <typename T>
void DenAsyncFrame2DBufferedWritter<T>::writeFrame(const Frame2DI<T>& f, uint32_t k)
{
    uint64_t position = offset + k * frameByteSize;
    std::lock_guard<std::mutex> guard(
        writingMutex); // Mutex will be released as this goes out of scope.
    if(XMajor)
    {
        for(uint32_t j = 0; j != sizey; j++)
        {
            for(uint32_t i = 0; i != sizex; i++)
            {
                util::setNextElement<T>(f(i, j), &buffer[(j * sizex + i) * sizeof(T)]);
            }
        }
    } else
    {
        for(uint32_t i = 0; i != sizex; i++)
        {
            for(uint32_t j = 0; j != sizey; j++)
            {
                util::setNextElement<T>(f(i, j), &buffer[(i * sizey + j) * sizeof(T)]);
            }
        }
    }
    io::writeBytesFrom(ofstream, position, buffer, frameByteSize);
    return;
}

template <typename T>
void DenAsyncFrame2DBufferedWritter<T>::writeBufferedFrame(BufferedFrame2D<T>& f, uint32_t k)
{
    uint64_t position = offset + k * frameByteSize;
    std::lock_guard<std::mutex> guard(
        writingMutex); // Mutex will be released as this goes out of scope.
    T* f_array = f.getDataPointer();
    if(XMajor && littleEndianArchitecture)
    {
        io::writeBytesFrom(ofstream, position, (uint8_t*)f.getDataPointer(), frameByteSize);
    } else
    {
        if(XMajor)
        {
            for(uint64_t i = 0; i != frameSize; i++)
            {
                util::setNextElement<T>(*(f_array + i), &buffer[sizeof(T) * i]);
            }
        } else
        {
            for(uint32_t i = 0; i != sizex; i++)
            {
                for(uint32_t j = 0; j != sizey; j++)
                {
                    util::setNextElement<T>(*(f_array + j * sizex + i),
                                            &buffer[(i * sizey + j) * sizeof(T)]);
                }
            }
        }
        io::writeBytesFrom(ofstream, position, buffer, frameByteSize);
    }
}

} // namespace KCT::io
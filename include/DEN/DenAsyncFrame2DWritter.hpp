#pragma once

// External libraries
#include <mutex>
#include <string>
#include <typeinfo>

// Internal libraries
#include "AsyncFrame2DWritterI.hpp"
#include "DEN/DenFileInfo.hpp"
#include "littleEndianAlignment.h"
#include "rawop.h"

namespace KCT::io {
/**
Interface for writing images. It is not necessery to write matrices along them.
*/
template <typename T>
class DenAsyncFrame2DWritter : public AsyncFrame2DWritterI<T>
{
private:
    std::string denFile;
    uint32_t sizex, sizey, sizez;
    uint64_t offset;
    bool extended;
    bool XMajor;
    uint8_t* buffer;
    mutable std::mutex writingMutex;

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
    DenAsyncFrame2DWritter(
        std::string denFile, uint32_t sizex, uint32_t sizey, uint32_t sizez, bool XMajor = true);

    /**
     * Constructor using file name of existing DEN file. It does not imediatelly overwrite or
     * zero the file.
     *
     * @param denFile
     */
    DenAsyncFrame2DWritter(std::string denFile);

    /**Writes i-th frame to the file.*/
    void writeFrame(const Frame2DI<T>& s, uint32_t i) override;

    /**Returns x dimension.*/
    virtual uint32_t dimx() const override;

    /**Returns y dimension.*/
    virtual uint32_t dimy() const override;

    /**Returns z dimension.*/
    virtual uint32_t dimz() const override;

    /**Returns file name.**/
    std::string getFileName() const;

    /// Destructor
    ~DenAsyncFrame2DWritter();
    /// Copy constructor
    DenAsyncFrame2DWritter(const DenAsyncFrame2DWritter<T>& b);
    // Copy assignment
    DenAsyncFrame2DWritter<T>& operator=(const DenAsyncFrame2DWritter<T> b);
    // Swap
    static void swap(DenAsyncFrame2DWritter<T>& a, DenAsyncFrame2DWritter<T>& b);
    // Move constructor
    DenAsyncFrame2DWritter(DenAsyncFrame2DWritter<T>&& b);
    // Move assignment
    DenAsyncFrame2DWritter<T>& operator=(DenAsyncFrame2DWritter<T>&& other);
};

template <typename T>
DenAsyncFrame2DWritter<T>::DenAsyncFrame2DWritter(
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
        try
        {
            DenFileInfo inf(denFile);
            if(inf.isValid() && inf.dimCount() == 3 && inf.getDataType() == type
               && inf.hasXMajorAlignment() == XMajor)
            {
                LOGD << io::xprintf("Will be writting to existing file %s.", denFile.c_str());
                offset = inf.getOffset();
            } else
            {
                DenFileInfo::createEmpty3DDenFile(denFile, type, dimx, dimy, dimz, XMajor);
                LOGD << io::xprintf("Just overwritten the file %s with empty file.",
                                    denFile.c_str());
            }
        } catch(util::KCTException e)
        {
            DenFileInfo::createEmpty3DDenFile(denFile, type, dimx, dimy, dimz, XMajor);
            LOGD << io::xprintf("Just overwritten the file %s with empty file.", denFile.c_str());
        }
    } else
    {
        DenFileInfo::createEmpty3DDenFile(denFile, type, dimx, dimy, dimz, XMajor);
    }
    buffer = new uint8_t[sizeof(T) * this->dimx() * this->dimy()];
}

template <typename T>
DenAsyncFrame2DWritter<T>::DenAsyncFrame2DWritter(std::string denFile)
{
    std::string err;
    if(!io::pathExists(denFile))
    {
        err = io::xprintf("The file %s does not exist.", denFile.c_str());
        LOGE << err;
        throw std::runtime_error(err);
    }
    io::DenFileInfo info(denFile);
    if(!info.isValid())
    {
        err = io::xprintf("The file %s is not valid DEN.", denFile.c_str());
        LOGE << err;
        throw std::runtime_error(err);
    }
    uint64_t elementByteSize = sizeof(T);
    if(info.elementByteSize() != elementByteSize)
    {
        err = io::xprintf("Element byte size %d of %s is incompatible with the size %d of "
                          "current template type.",
                          info.elementByteSize(), denFile.c_str(), elementByteSize);
        LOGE << err;
        throw std::runtime_error(err);
    }
    extended = info.isExtended();
    offset = info.getOffset();
    this->denFile = denFile;
    this->sizex = info.dimx();
    this->sizey = info.dimy();
    this->sizez = info.dimz();
    buffer = new uint8_t[sizeof(T) * this->sizex * this->sizey];
}

/// Guard for move constructor stealed objects
template <typename T>
DenAsyncFrame2DWritter<T>::~DenAsyncFrame2DWritter()
{
    if(buffer != nullptr)
        delete[] buffer;
    buffer = nullptr;
}

/// Copy constructor
template <typename T>
DenAsyncFrame2DWritter<T>::DenAsyncFrame2DWritter(const DenAsyncFrame2DWritter<T>& b)
    : DenAsyncFrame2DWritter<T>::DenAsyncFrame2DWritter(b.denFile, b.dimx, b.dimy, b.dimz)
{
    LOGD << "Caling Copy constructor of DenAsyncFrame2DWritter";
}

// Copy assignment
template <typename T>
DenAsyncFrame2DWritter<T>& DenAsyncFrame2DWritter<T>::operator=(const DenAsyncFrame2DWritter<T> b)
{
    LOGD << "Caling Copy assignment constructor of DenAsyncFrame2DWritter";
    swap(*this, b);
    return *this;
}

template <typename T>
void DenAsyncFrame2DWritter<T>::swap(DenAsyncFrame2DWritter<T>& a, DenAsyncFrame2DWritter<T>& b)
{
    std::swap(a.denFile, b.denFile);
    std::swap(a.sizex, b.sizex);
    std::swap(a.sizey, b.sizey);
    std::swap(a.sizez, b.sizez);
    std::swap(a.offset, b.offset);
    std::swap(a.extended, b.extended);
    std::swap(a.buffer, b.buffer);
}

// Move constructor
template <typename T>
DenAsyncFrame2DWritter<T>::DenAsyncFrame2DWritter(DenAsyncFrame2DWritter<T>&& b)
{
    LOGD << "Caling Move constructor of DenAsyncFrame2DWritter";
    this->denFile = b.denFile;
    this->sizex = b.sizex;
    this->sizey = b.sizey;
    this->sizez = b.sizez;
    this->offset = b.offset;
    this->extended = b.extended;
    this->buffer = std::exchange(b.buffer, nullptr);
}

// Move assignment
template <typename T>
DenAsyncFrame2DWritter<T>& DenAsyncFrame2DWritter<T>::operator=(DenAsyncFrame2DWritter<T>&& b)
{
    LOGD << "Caling Move assignment constructor of DenAsyncFrame2DWritter";
    if(&b != this) // To elegantly solve situation when assigning to itself
    {
        this->denFile = b.denFile;
        this->sizex = b.sizex;
        this->sizey = b.sizey;
        this->sizez = b.sizez;
        this->offset = b.offset;
        this->extended = b.extended;
        if(this->buffer != nullptr)
        {
            delete[] this->buffer;
            this->buffer = nullptr;
        }
        this->buffer = std::exchange(b.buffer, nullptr);
    }
}

template <typename T>
std::string DenAsyncFrame2DWritter<T>::getFileName() const
{
    return this->denFile;
}

template <typename T>
uint32_t DenAsyncFrame2DWritter<T>::dimx() const
{
    return sizex;
}

template <typename T>
uint32_t DenAsyncFrame2DWritter<T>::dimy() const
{
    return sizey;
}

template <typename T>
uint32_t DenAsyncFrame2DWritter<T>::dimz() const
{
    return sizez;
}

template <typename T>
void DenAsyncFrame2DWritter<T>::writeFrame(const Frame2DI<T>& s, uint32_t i)
{
    std::lock_guard<std::mutex> guard(
        writingMutex); // Mutex will be released as this goes out of scope.
    for(std::size_t j = 0; j != this->dimy(); j++)
    {
        for(std::size_t k = 0; k != this->dimx(); k++)
        {
            if(XMajor)
            {
                util::setNextElement<T>(s(k, j), &buffer[(j * this->dimx() + k) * sizeof(T)]);
            } else
            {
                util::setNextElement<T>(s(k, j), &buffer[(k * this->dimy() + j) * sizeof(T)]);
            }
        }
    }
    uint64_t position = offset + ((uint64_t)sizeof(T)) * i * this->dimx() * this->dimy();
    io::writeBytesFrom(denFile, position, buffer, sizeof(T) * this->dimx() * this->dimy());
    return;
}

} // namespace KCT::io

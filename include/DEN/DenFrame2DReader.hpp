#pragma once

// External
#include <mutex>
#include <string>

// Internal
#include "BufferedFrame2D.hpp"
#include "DEN/DenFileInfo.hpp"
#include "Frame2DI.hpp"
#include "Frame2DReaderI.hpp"

namespace KCT::io {
/**
 * Implementation of the ProjectionReader for projections and projection matrices stored in the
 * den files.
 */
template <typename T>
class DenFrame2DReader : virtual public Frame2DReaderI<T>
// Frame2DReaderI<T> will be only once in the family tree
{

public:
    /**Constructs DenFrame2DReader from file name.
     *
     *
     * @param denFile
     * @param denFile File in a DEN format to read by frames.
     * @param additionalBufferNum Number of additional buffers to allocate. Defaults to 0 for 1
     * default buffer.
     */
    DenFrame2DReader(std::string denFile, uint32_t additionalBufferNum = 0);
    /// Destructor
    ~DenFrame2DReader();
    /// Copy constructor
    DenFrame2DReader(const DenFrame2DReader<T>& b) = delete;
    // Copy assignment with copy and swap
    DenFrame2DReader<T>& operator=(DenFrame2DReader<T>& b) = delete;
    // Move constructor
    DenFrame2DReader(DenFrame2DReader<T>&& b) = delete;
    // Move assignment
    DenFrame2DReader<T>& operator=(DenFrame2DReader<T>&& other) = delete;
    std::shared_ptr<io::Frame2DI<T>> readFrame(uint64_t k) override;
    std::shared_ptr<io::BufferedFrame2D<T>> readBufferedFrame(uint64_t k);
    void
    readFrameIntoBuffer(uint64_t flatFrameIndex, T* outside_buffer, bool XMajorAlignment = true);
    uint32_t dimx() const override;
    uint32_t dimy() const override;
    uint64_t getFrameCount() const override;
    uint64_t getFrameSize() const override;
    uint64_t getFrameByteSize() const override;
    std::string getFileName() const;
    /**Returns file name of the underlying DEN file.**/
    DenSupportedType getDataType() const;

protected:
    // protected: // Visible in inheritance structure
    std::string denFile;
    uint64_t offset;
    bool XMajorAlignment;
    uint32_t sizex, sizey;
    uint64_t frameSize;
    uint64_t frameByteSize;
    uint64_t frameCount;
    DenSupportedType dataType;
    uint64_t elementByteSize;

private:
    mutable std::mutex* consistencyMutexes;
    uint8_t** buffers;
    T** buffer_copys;
    uint32_t additionalBufferNum;
};

template <typename T>
DenFrame2DReader<T>::DenFrame2DReader(std::string denFile, uint32_t additionalBufferNum)
    : denFile(denFile)
    , additionalBufferNum(additionalBufferNum)
{
    std::string ERR;
    DenFileInfo pi = DenFileInfo(this->denFile);
    this->dataType = pi.getElementType();
    DenSupportedType readerDataType = getDenSupportedTypeByTypeID(typeid(T));
    if(dataType != readerDataType)
    {
        ERR = io::xprintf("The file %s of the type %s is to be read by reader of type %s!",
                          denFile.c_str(), DenSupportedTypeToString(dataType).c_str(),
                          DenSupportedTypeToString(readerDataType).c_str());
        LOGW << ERR;
    }
    this->offset = pi.getOffset();
    this->sizex = pi.dimx();
    this->sizey = pi.dimy();
    this->elementByteSize = pi.getElementByteSize();
    this->frameCount = pi.getFrameCount();
    this->frameSize = pi.getFrameSize();
    this->frameByteSize = pi.getFrameByteSize();
    this->XMajorAlignment = pi.hasXMajorAlignment();
    this->consistencyMutexes = new std::mutex[1 + additionalBufferNum];
    this->buffers = new uint8_t*[1 + additionalBufferNum];
    this->buffer_copys = new T*[1 + additionalBufferNum];
    for(uint32_t i = 0; i != 1 + additionalBufferNum; i++)
    {
        this->buffers[i] = new uint8_t[elementByteSize * sizex * sizey];
        this->buffer_copys[i] = new T[sizex * sizey];
    }
    // Buffers are used for the alocation of new frames. Since this class uses the
    // instance that copies memory, this memory might me reused.
}

// This mumbo jumbo is for correctly deleting object after move assignment operation was
// performed to be able to delete such object with stealed internals where its buffers are
// nullptr.
template <typename T>
DenFrame2DReader<T>::~DenFrame2DReader()
{
    if(consistencyMutexes != nullptr)
    {
        delete[] consistencyMutexes;
    }
    consistencyMutexes = nullptr;
    if(buffers != nullptr)
    {
        for(uint32_t i = 0; i != 1 + additionalBufferNum; i++)
        {
            if(buffers[i] != nullptr)
            {
                delete buffers[i];
            }
            buffers[i] = nullptr;
        }
        delete[] buffers;
    }
    buffers = nullptr;
    if(buffer_copys != nullptr)
    {
        for(uint32_t i = 0; i != 1 + additionalBufferNum; i++)
        {
            if(buffer_copys[i] != nullptr)
            {
                delete buffer_copys[i];
            }
            buffer_copys[i] = nullptr;
        }
        delete[] buffer_copys;
    }
    buffer_copys = nullptr;
}

template <typename T>
std::string DenFrame2DReader<T>::getFileName() const
{
    return this->denFile;
}

template <typename T>
uint32_t DenFrame2DReader<T>::dimx() const
{
    return sizex;
}

template <typename T>
uint32_t DenFrame2DReader<T>::dimy() const
{
    return sizey;
}

template <typename T>
uint64_t DenFrame2DReader<T>::getFrameCount() const
{
    return frameCount;
}

template <typename T>
uint64_t DenFrame2DReader<T>::getFrameSize() const
{
    return frameSize;
}

template <typename T>
uint64_t DenFrame2DReader<T>::getFrameByteSize() const
{
    return frameByteSize;
}

template <typename T>
std::shared_ptr<io::Frame2DI<T>> DenFrame2DReader<T>::readFrame(uint64_t k)
{
    std::shared_ptr<Frame2DI<T>> f = readBufferedFrame(k);
    return f;
}

template <typename T>
std::shared_ptr<io::BufferedFrame2D<T>> DenFrame2DReader<T>::readBufferedFrame(uint64_t k)
{
    std::unique_lock<std::mutex> l;
    bool locked = false;
    uint32_t mutexnum = 0;
    for(uint32_t i = 0; i != 1 + additionalBufferNum; i++)
    {
        l = std::unique_lock<std::mutex>(consistencyMutexes[i], std::try_to_lock);
        if(l.owns_lock())
        {
            locked = true;
            mutexnum = i;
            break;
        }
    }
    if(!locked)
    {
        l = std::unique_lock<std::mutex>(consistencyMutexes[0]);
        mutexnum = 0;
    }
    // Mutex will be released as this goes out of scope.
    // To protect calling this method from another thread using the same block of memory
    uint8_t* buffer = buffers[mutexnum];
    T* buffer_copy = buffer_copys[mutexnum];
    uint64_t position = this->offset + k * frameByteSize;
    io::readBytesFrom(this->denFile, position, buffer, frameByteSize);
    if(this->XMajorAlignment)
    {
        for(uint64_t a = 0; a != frameSize; a++)
        {
            buffer_copy[a] = util::getNextElement<T>(&buffer[a * elementByteSize], dataType);
        }
    } else
    { // Frame2D is implemented as row major container
        for(uint64_t x = 0; x != sizex; x++)
        {
            for(uint64_t y = 0; y != sizey; y++)
            {
                buffer_copy[x + sizex * y]
                    = util::getNextElement<T>(&buffer[(y + sizey * x) * elementByteSize], dataType);
            }
        }
    }
    std::shared_ptr<BufferedFrame2D<T>> f
        = std::make_shared<BufferedFrame2D<T>>(buffer_copy, sizex, sizey);
    return f;
}

template <typename T>
void DenFrame2DReader<T>::readFrameIntoBuffer(uint64_t k,
                                              T* outside_buffer,
                                              bool XMajorAlignment)
{
    std::unique_lock<std::mutex> l;
    bool locked = false;
    uint32_t mutexnum = 0;
    for(uint32_t i = 0; i != 1 + additionalBufferNum; i++)
    {
        l = std::unique_lock<std::mutex>(consistencyMutexes[i], std::try_to_lock);
        if(l.owns_lock())
        {
            locked = true;
            mutexnum = i;
            break;
        }
    }
    if(!locked)
    {
        l = std::unique_lock<std::mutex>(consistencyMutexes[0]);
        mutexnum = 0;
    }
    // Mutex will be released as this goes out of scope.
    // To protect calling this method from another thread using the same block of memory
    uint8_t* buffer = buffers[mutexnum];
    uint64_t position = this->offset + k * frameByteSize;
    io::readBytesFrom(this->denFile, position, buffer, elementByteSize * frameSize);
    if(XMajorAlignment == this->XMajorAlignment)
    {
        for(uint64_t a = 0; a != frameSize; a++)
        {
            outside_buffer[a] = util::getNextElement<T>(&buffer[a * elementByteSize], dataType);
        }
    } else
    {
        uint64_t innerIndex, outerIndex;
        for(uint64_t x = 0; x != sizex; x++)
        {
            for(uint64_t y = 0; y != sizey; y++)
            {
                if(this->XMajorAlignment)
                {
                    innerIndex = x + sizex * y;
                } else
                {
                    innerIndex = y + sizey * x;
                }
                if(XMajorAlignment)
                {
                    outerIndex = x + sizex * y;
                } else
                {
                    outerIndex = y + sizey * x;
                }
                outside_buffer[outerIndex]
                    = util::getNextElement<T>(&buffer[innerIndex * elementByteSize], dataType);
            }
        }
    }
}

template <typename T>
DenSupportedType DenFrame2DReader<T>::getDataType() const
{
    return dataType;
}
} // namespace KCT::io

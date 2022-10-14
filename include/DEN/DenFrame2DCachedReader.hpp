#pragma once

// External
#include <algorithm>
#include <mutex>
#include <queue>
#include <string>

// Internal
#include "BufferedFrame2D.hpp"
#include "DEN/DenFileInfo.hpp"
#include "Frame2DI.hpp"
#include "Frame2DReaderI.hpp"

namespace KCT::io {
/**
 * Thread safe implementation of the ProjectionReader for projections and projection matrices stored
 * in the den files.
 */
template <typename T>
class DenFrame2DCachedReader : virtual public Frame2DReaderI<T>
// Frame2DReaderI<T> will be only once in the family tree
{

public:
    /**Constructs DenFrame2DCachedReader from file name.
     *
     *
     * @param denFile
     * @param denFile File in a DEN format to read by frames.
     * @param additionalBufferNum Number of additional buffers to allocate. Defaults to 0 for 1
     * default buffer.
     * @param cache_size Store cache_size frames in memory
     */
    DenFrame2DCachedReader(std::string denFile,
                           uint32_t additionalBufferNum = 0,
                           uint32_t cache_size = 0);
    /// Destructor
    ~DenFrame2DCachedReader();
    /// Copy constructor
    DenFrame2DCachedReader(const DenFrame2DCachedReader<T>& b) = delete;
    // Copy assignment with copy and swap
    DenFrame2DCachedReader<T>& operator=(DenFrame2DCachedReader<T>& b) = delete;
    // Move constructor
    DenFrame2DCachedReader(DenFrame2DCachedReader<T>&& b) = delete;
    // Move assignment
    DenFrame2DCachedReader<T>& operator=(DenFrame2DCachedReader<T>&& other) = delete;
    std::shared_ptr<io::Frame2DI<T>> readFrame(uint32_t k) override;
    std::shared_ptr<io::BufferedFrame2D<T>> readBufferedFrame(uint32_t k);
    void frameToCache(uint32_t k, std::shared_ptr<BufferedFrame2D<T>> f);
    /**
     * Populate cache by frameCount frames.
     *
     * @param fromID Index to start populating cache.
     * @param frameCount Number of frames to put into the cache.
     */
    void fillCache(uint32_t fromID, uint32_t frameCount);
    void
    readFrameIntoBuffer(uint64_t flatFrameIndex, T* outside_buffer, bool XMajorAlignment = true);
    uint32_t dimx() const override;
    uint32_t dimy() const override;
    uint32_t dimz() const override;
    uint64_t dimflatz() const;
    std::string getFileName() const;
    /**Returns file name of the underlying DEN file.**/
    DenSupportedType getDataType() const;

protected:
    // protected: // Visible in inheritance structure
    std::string denFile;
    uint64_t offset;
    bool XMajorAlignment;
    uint64_t sizex, sizey, sizez;
    uint64_t frameSize, frameByteSize;
    DenSupportedType dataType;
    uint16_t elementByteSize;

private:
    mutable std::mutex* consistencyMutexes;
    mutable std::mutex cacheConsistencyMutex;
    uint8_t** buffers;
    T** buffer_copys;
    std::vector<std::shared_ptr<BufferedFrame2D<T>>> cache;
    std::queue<uint32_t> cachedFramesQueue;
    uint32_t bufferCount;
    uint32_t cacheSize;
    bool littleEndianArchitecture;

    void initialize();
};

template <typename T>
void DenFrame2DCachedReader<T>::initialize()
{
    int num = 1;
    littleEndianArchitecture = (*(char*)&num == 1);
}

template <typename T>
DenFrame2DCachedReader<T>::DenFrame2DCachedReader(std::string denFile,
                                                  uint32_t additionalBufferNum,
                                                  uint32_t cacheSize)
    : denFile(denFile)
    , bufferCount(additionalBufferNum + 1)
    , cacheSize(cacheSize)
{
    DenFileInfo pi = DenFileInfo(this->denFile);
    this->offset = pi.getOffset();
    this->sizex = pi.dimx();
    this->sizey = pi.dimy();
    this->sizez = pi.getFrameCount();
    this->frameSize = pi.getFrameSize();
    this->elementByteSize = pi.getElementByteSize();
    this->frameByteSize = pi.getFrameByteSize();
    this->XMajorAlignment = pi.hasXMajorAlignment();
    this->dataType = pi.getElementType();
    this->consistencyMutexes = new std::mutex[bufferCount];
    this->buffers = new uint8_t*[bufferCount];
    this->buffer_copys = new T*[bufferCount];
    if(this->cacheSize > this->sizez)
    {
        this->cacheSize = this->sizez;
    }
    this->cache.resize(this->sizez, nullptr); // Initialized by nullptrs
    for(uint32_t i = 0; i != bufferCount; i++)
    {
        this->buffers[i] = new uint8_t[elementByteSize * sizex * sizey];
        this->buffer_copys[i] = new T[sizex * sizey];
    }
    // Buffers are used for the alocation of new frames. Since this class uses the
    // instance that copies memory, this memory might me reused.
    initialize();
}

// This mumbo jumbo is for correctly deleting object after move assignment operation was
// performed to be able to delete such object with stealed internals where its buffers are
// nullptr.
template <typename T>
DenFrame2DCachedReader<T>::~DenFrame2DCachedReader()
{
    if(consistencyMutexes != nullptr)
    {
        delete[] consistencyMutexes;
    }
    consistencyMutexes = nullptr;
    if(buffers != nullptr)
    {
        for(uint32_t i = 0; i != bufferCount; i++)
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
        for(uint32_t i = 0; i != bufferCount; i++)
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
std::string DenFrame2DCachedReader<T>::getFileName() const
{
    return this->denFile;
}

template <typename T>
uint32_t DenFrame2DCachedReader<T>::dimx() const
{
    return sizex;
}

template <typename T>
uint32_t DenFrame2DCachedReader<T>::dimy() const
{
    return sizey;
}

template <typename T>
uint32_t DenFrame2DCachedReader<T>::dimz() const
{
    return sizez;
}

template <typename T>
void DenFrame2DCachedReader<T>::fillCache(uint32_t fromID, uint32_t frameCount)
{
    if(fromID >= sizez)
    {
        LOGW << io::xprintf("fromID=%d is greater or equal than sizez=%d", fromID, sizez);
        return;
    }
    if(cacheSize != 0)
    {
        if(frameCount < cacheSize)
        {
            LOGW << io::xprintf(
                "Specified frameCount=%d exceeds cacheSize=%d, setting frameCount to cacheSize.");
            frameCount = cacheSize;
        }
        for(uint64_t k = fromID; k != std::min(uint64_t(fromID) + frameCount, sizez); k++)
        {
            readBufferedFrame(k);
        }
    } else
    {
        LOGW << io::xprintf(
            "There was no cache specified when creating this object, can not fill.");
    }
}

template <typename T>
std::shared_ptr<io::Frame2DI<T>> DenFrame2DCachedReader<T>::readFrame(uint32_t k)
{
    std::shared_ptr<Frame2DI<T>> f = readBufferedFrame(k);
    return f;
}

template <typename T>
std::shared_ptr<io::BufferedFrame2D<T>> DenFrame2DCachedReader<T>::readBufferedFrame(uint32_t k)
{
    std::shared_ptr<BufferedFrame2D<T>> f;
    // If it is in cache, return it directly
    f = cache[k];
    if(f != nullptr)
    {
        return f;
    }
    // If not start fetching
    std::unique_lock<std::mutex> l;
    bool locked = false;
    uint32_t mutexnum = 0;
    for(uint32_t i = 0; i != 1 + bufferCount; i++)
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
    if(this->XMajorAlignment && this->littleEndianArchitecture)
    {
        io::readBytesFrom(this->denFile, position, (uint8_t*)buffer_copy, frameByteSize);
        f = std::make_shared<BufferedFrame2D<T>>(buffer_copy, sizex, sizey);
    } else
    {

        io::readBytesFrom(this->denFile, position, buffer, frameByteSize);
        if(this->XMajorAlignment)
        {
            for(uint64_t a = 0; a != frameSize; a++)
            {
                buffer_copy[a] = util::getNextElement<T>(&buffer[a * elementByteSize], dataType);
            }
        } else
        { // Frame2D is implemented as row major container
            for(uint32_t x = 0; x != sizex; x++)
            {
                for(uint32_t y = 0; y != sizey; y++)
                {
                    buffer_copy[x + sizex * y] = util::getNextElement<T>(
                        &buffer[(y + sizey * x) * elementByteSize], dataType);
                }
            }
        }
        f = std::make_shared<BufferedFrame2D<T>>(buffer_copy, sizex, sizey);
    }
    frameToCache(k, f);
    return f;
}

template <typename T>
void DenFrame2DCachedReader<T>::frameToCache(uint32_t k, std::shared_ptr<BufferedFrame2D<T>> f)
{
    if(cacheSize > 0)
    {
        cache[k] = f;
        cachedFramesQueue.push(k);
        if(cachedFramesQueue.size() > cacheSize && !cachedFramesQueue.empty())
        {
            std::unique_lock<std::mutex> _synchronized_block(cacheConsistencyMutex);
            while(cachedFramesQueue.size() > cacheSize && !cachedFramesQueue.empty())
            {

                uint32_t k_delete = cachedFramesQueue.front();
                cachedFramesQueue.pop();
                cache[k_delete] = nullptr;
            }
        }
    }
}

template <typename T>
void DenFrame2DCachedReader<T>::readFrameIntoBuffer(uint64_t k,
                                                    T* outside_buffer,
                                                    bool XMajorAlignment)
{
    std::unique_lock<std::mutex> l;
    bool locked = false;
    uint32_t mutexnum = 0;
    for(uint32_t i = 0; i != 1 + bufferCount; i++)
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
    io::readBytesFrom(this->denFile, position, buffer, frameByteSize);
    if(XMajorAlignment == this->XMajorAlignment)
    {
        for(uint32_t a = 0; a != frameSize; a++)
        {
            outside_buffer[a] = util::getNextElement<T>(&buffer[a * elementByteSize], dataType);
        }
    } else
    {
        uint32_t innerIndex, outerIndex;
        for(uint32_t x = 0; x != sizex; x++)
        {
            for(uint32_t y = 0; y != sizey; y++)
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
DenSupportedType DenFrame2DCachedReader<T>::getDataType() const
{
    return dataType;
}
} // namespace KCT::io

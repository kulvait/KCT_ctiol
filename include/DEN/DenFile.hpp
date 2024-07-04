#pragma once

// External
#include <atomic>
#include <memory>
#include <mutex>
#include <string>
#include <thread>
#include <vector>

// Internal
#include "BufferedFrame2D.hpp"
#include "DEN/DenFileInfo.hpp"
#include "Frame2DI.hpp"
#include "Frame2DReaderI.hpp"

namespace KCT::io {

/**
 * Class to read the entire DEN file into memory and provide access to individual frames.
 */
template <typename T>
class DenFile
{
public:
    DenFile(std::string denFile, uint32_t numThreads = std::thread::hardware_concurrency());
    ~DenFile();

    std::shared_ptr<io::Frame2DI<T>> getFrame(uint64_t k);
    uint32_t dimx() const;
    uint32_t dimy() const;
    uint64_t getFrameCount() const;
    uint64_t getFrameSize() const;
    uint64_t getFrameByteSize() const;
    std::string getFileName() const;
    DenSupportedType getDataType() const;

    void writeFile(std::string fileName, bool overwrite = false);

    // Pointers and iterators
    T* getDataPointer();
    T* getFramePointer(uint64_t k);
    typename std::vector<T>::iterator begin();
    typename std::vector<T>::iterator end();
    typename std::vector<T>::iterator beginFrame(uint64_t k);
    typename std::vector<T>::iterator endFrame(uint64_t k);

private:
    void readFileIntoMemory();
    void readFileChunk(uint64_t startFrame, uint64_t endFrame);
    void writeFileChunk(std::string fileName, uint64_t startFrame, uint64_t endFrame);

    std::string denFile;
    DenFileInfo denFileInfo;
    uint64_t offset;
    bool XMajorAlignment;
    uint32_t sizex, sizey;
    uint64_t frameSize;
    uint64_t frameByteSize;
    uint64_t frameCount;
    DenSupportedType dataType;
    uint64_t elementByteSize;

    std::vector<T> fileData;
    std::vector<uint64_t> frameOffsets;
    uint32_t numThreads;

    bool littleEndianArchitecture;
    std::atomic<bool> readCompleted;
    std::mutex readMutex;
};

template <typename T>
DenFile<T>::DenFile(std::string denFile, uint32_t numThreads)
    : denFile(denFile)
    , denFileInfo(denFile)
    , numThreads(numThreads)
    , readCompleted(false)
{
    std::string ERR;
    this->dataType = denFileInfo.getElementType();
    DenSupportedType readerDataType = getDenSupportedTypeByTypeID(typeid(T));
    if(dataType != readerDataType)
    {
        ERR = io::xprintf("The file %s of the type %s is to be read by reader of type %s!",
                          denFile.c_str(), DenSupportedTypeToString(dataType).c_str(),
                          DenSupportedTypeToString(readerDataType).c_str());
        LOGW << ERR;
    }
    this->offset = denFileInfo.getOffset();
    this->sizex = denFileInfo.dimx();
    this->sizey = denFileInfo.dimy();
    this->elementByteSize = denFileInfo.getElementByteSize();
    this->frameCount = denFileInfo.getFrameCount();
    this->frameSize = denFileInfo.getFrameSize();
    this->frameByteSize = denFileInfo.getFrameByteSize();
    this->XMajorAlignment = denFileInfo.hasXMajorAlignment();

    frameOffsets.resize(frameCount);
    for(uint64_t i = 0; i < frameCount; ++i)
    {
        frameOffsets[i] = offset + i * frameByteSize;
    }

    fileData.resize(frameSize * frameCount);

    int num = 1;
    littleEndianArchitecture = (*(char*)&num == 1);

    readFileIntoMemory();
}

template <typename T>
DenFile<T>::~DenFile()
{
    // Destructor
}

template <typename T>
void DenFile<T>::readFileIntoMemory()
{
    if(numThreads <= 1)
    {
        readFileChunk(0, frameCount);
        readCompleted.store(true);
        return;
    }

    std::vector<std::thread> async_threads;
    uint64_t threads = std::min(static_cast<uint64_t>(numThreads), frameCount);
    uint64_t framesPerThread = (frameCount + threads - 1) / threads;

    for(uint32_t i = 0; i < threads; ++i)
    {
        uint64_t startFrame = i * framesPerThread;
        uint64_t endFrame = std::min(startFrame + framesPerThread, frameCount);
        async_threads.emplace_back(&DenFile::readFileChunk, this, startFrame, endFrame);
    }

    for(auto& thread : async_threads)
    {
        thread.join();
    }

    readCompleted.store(true);
}

template <typename T>
void DenFile<T>::readFileChunk(uint64_t startFrame, uint64_t endFrame)
{
    uint8_t* buffer = new uint8_t[frameByteSize];
    for(uint64_t k = startFrame; k < endFrame; ++k)
    {
        uint64_t position = frameOffsets[k];
        io::readBytesFrom(this->denFile, position, buffer, frameByteSize);

        if(littleEndianArchitecture)
        {
            std::memcpy(&fileData[k * frameSize], buffer, frameByteSize);
        } else
        {
            for(uint64_t a = 0; a != frameSize; a++)
            {
                fileData[k * frameSize + a]
                    = util::getNextElement<T>(&buffer[a * elementByteSize], dataType);
            }
        }
    }
    delete[] buffer;
}

template <typename T>
void DenFile<T>::writeFileChunk(std::string fileName, uint64_t startFrame, uint64_t endFrame)
{
    uint64_t fileOffset = 0;
    T* pointer = fileData.data();
    T* framePointer = nullptr;
    if(littleEndianArchitecture)
    {
        for(uint64_t k = startFrame; k < endFrame; ++k)
        {
            fileOffset = frameOffsets[k];
            framePointer = pointer + k * frameSize;
            io::writeBytesFrom(fileName, fileOffset, (uint8_t*)framePointer, frameByteSize);
        }
    } else
    {

        uint8_t* buffer = new uint8_t[frameByteSize];
        for(uint64_t k = startFrame; k < endFrame; ++k)
        {
            fileOffset = frameOffsets[k];
            framePointer = pointer + k * frameSize;
            for(uint64_t a = 0; a != frameSize; a++)
            {
                util::setNextElement<T>(*framePointer, &buffer[a * elementByteSize]);
            }
            io::writeBytesFrom(fileName, fileOffset, buffer, frameByteSize);
        }
        delete[] buffer;
    }
}

template <typename T>
void DenFile<T>::writeFile(std::string fileName, bool force)
{
    // Create array with the dimensions of the file
    std::vector<uint32_t> dims;
    for(uint32_t i = 0; i != denFileInfo.getDimCount(); i++)
    {
        dims.push_back(denFileInfo.dim(i));
    }
    if(io::pathExists(fileName))
    {
        DenFileInfo inf(fileName, false);
        bool compatibleFiles = false;
        if(inf.isValid() && inf.getDimCount() == denFileInfo.getDimCount()
           && inf.getElementType() == dataType && inf.hasXMajorAlignment() == XMajorAlignment)
        {
            compatibleFiles = true;
            for(uint32_t i = 0; i != inf.getDimCount(); i++)
            {
                if(inf.dim(i) != dims[i])
                {
                    compatibleFiles = false;
                    break;
                }
            }
        }
        if(force)
        {
            if(compatibleFiles)
            {
                LOGI << io::xprintf("The file %s already exists and is compatible with the current "
                                    "data, overwriting its content.",
                                    fileName.c_str());
            } else
            {

                LOGW << io::xprintf(
                    "The file %s already exists and is not compatible with the current "
                    "data, removing it and writing new file.",
                    fileName.c_str());
                DenFileInfo::createEmptyDenFile(fileName, dataType, dims.size(), dims.data(),
                                                XMajorAlignment);
            }
        } else
        {
            std::string err = io::xprintf(
                "The file %s already exists, add force = true to overwrite it.", fileName.c_str());
        }
    } else
    {
        DenFileInfo::createEmptyDenFile(fileName, dataType, dims.size(), dims.data(),
                                        XMajorAlignment);
    }
    if(numThreads <= 1)
    {
        writeFileChunk(fileName, 0, frameCount);
        return;
    }

    std::vector<std::thread> async_threads;
    uint64_t threads = std::min(static_cast<uint64_t>(numThreads), frameCount);
    uint64_t framesPerThread = (frameCount + threads - 1) / threads;

    for(uint32_t i = 0; i < threads; ++i)
    {
        uint64_t startFrame = i * framesPerThread;
        uint64_t endFrame = std::min(startFrame + framesPerThread, frameCount);
        async_threads.emplace_back(&DenFile::writeFileChunk, this, fileName, startFrame, endFrame);
    }

    for(auto& thread : async_threads)
    {
        thread.join();
    }
}

template <typename T>
std::shared_ptr<io::Frame2DI<T>> DenFile<T>::getFrame(uint64_t k)
{
    if(!readCompleted.load())
    {
        std::lock_guard<std::mutex> lock(readMutex);
    }

    T* buffer_copy = new T[frameSize];
    if(this->XMajorAlignment)
    {
        std::memcpy(buffer_copy, &fileData[k * frameSize], frameSize * sizeof(T));
    } else
    {
        for(uint64_t x = 0; x != sizex; x++)
        {
            for(uint64_t y = 0; y != sizey; y++)
            {
                buffer_copy[x + sizex * y] = fileData[k * frameSize + (y + sizey * x)];
            }
        }
    }
    std::shared_ptr<io::Frame2DI<T>> frame
        = std::make_shared<BufferedFrame2D<T>>(buffer_copy, sizex, sizey);
    return frame;
}

template <typename T>
uint32_t DenFile<T>::dimx() const
{
    return sizex;
}

template <typename T>
uint32_t DenFile<T>::dimy() const
{
    return sizey;
}

template <typename T>
uint64_t DenFile<T>::getFrameCount() const
{
    return frameCount;
}

template <typename T>
uint64_t DenFile<T>::getFrameSize() const
{
    return frameSize;
}

template <typename T>
uint64_t DenFile<T>::getFrameByteSize() const
{
    return frameByteSize;
}

template <typename T>
std::string DenFile<T>::getFileName() const
{
    return denFile;
}

template <typename T>
DenSupportedType DenFile<T>::getDataType() const
{
    return dataType;
}

template <typename T>
T* DenFile<T>::getDataPointer()
{
    return fileData.data();
}

template <typename T>
T* DenFile<T>::getFramePointer(uint64_t k)
{
    return fileData.data() + k * frameSize;
}

template <typename T>
typename std::vector<T>::iterator DenFile<T>::begin()
{
    return fileData.begin();
}

template <typename T>
typename std::vector<T>::iterator DenFile<T>::end()
{
    return fileData.end();
}

template <typename T>
typename std::vector<T>::iterator DenFile<T>::beginFrame(uint64_t k)
{
    return fileData.begin() + k * frameSize;
}

template <typename T>
typename std::vector<T>::iterator DenFile<T>::endFrame(uint64_t k)
{
    return fileData.begin() + (k + 1) * frameSize;
}

} // namespace KCT::io

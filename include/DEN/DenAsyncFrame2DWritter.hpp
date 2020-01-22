#pragma once

// External libraries
#include <mutex>
#include <string>

// Internal libraries
#include "AsyncFrame2DWritterI.hpp"
#include "littleEndianAlignment.h"
#include "rawop.h"

namespace CTL {
namespace io {
    /**
    Interface for writing images. It is not necessery to write matrices along them.
    */
    template <typename T>
    class DenAsyncFrame2DWritter : public AsyncFrame2DWritterI<T>
    {
    private:
        std::string projectionsFile;
        uint32_t sizex, sizey, sizez;
        uint64_t offset;
        bool extended;
        uint8_t* buffer;
        mutable std::mutex writingMutex;

    public:
        /*To write file is needed to specify its name and dimensions.*/
        DenAsyncFrame2DWritter(std::string projectionsFile,
                               uint32_t dimx,
                               uint32_t dimy,
                               uint32_t dimz);

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
        DenAsyncFrame2DWritter<T>& operator=(const DenAsyncFrame2DWritter<T>& b);
        // Move constructor
        DenAsyncFrame2DWritter(DenAsyncFrame2DWritter<T>&& b);
        // Move assignment
        DenAsyncFrame2DWritter<T>& operator=(DenAsyncFrame2DWritter<T>&& other);
    };

    template <typename T>
    DenAsyncFrame2DWritter<T>::DenAsyncFrame2DWritter(std::string projectionsFile,
                                                      uint32_t dimx,
                                                      uint32_t dimy,
                                                      uint32_t dimz)
    {
        if(dimx < 1 || dimy < 1 || dimz < 1)
        {
            std::string msg = io::xprintf("One of the dimensions is nonpositive x=%d, y=%d, z=%d.",
                                          dimx, dimy, dimz);
            LOGE << msg;
            throw std::runtime_error(msg);
        }
        extended = false;
        offset = 6;
        if(dimx > 65535 || dimy > 65535 || dimz > 65535)
        {
            extended = true;
            offset = 18;
        }
        this->projectionsFile = projectionsFile;
        this->sizex = dimx;
        this->sizey = dimy;
        this->sizez = dimz;
        uint64_t elementByteSize = sizeof(T);
        uint64_t totalFileSize = offset + elementByteSize * dimx * dimy * dimz;
        if(io::pathExists(projectionsFile))
        {
            uint64_t fileSize = io::getFileSize(projectionsFile);
            if(fileSize != totalFileSize)
            {
                io::createEmptyFile(projectionsFile, totalFileSize, true);
                LOGD << io::xprintf(
                    "Just overwritten the file %s with empty file of the size %ld bytes.",
                    projectionsFile.c_str(), totalFileSize);
            }
            LOGD << io::xprintf("Will be writting to existing file %s of %ld bytes.",
                                projectionsFile.c_str(), totalFileSize);
        } else
        {
            io::createEmptyFile(projectionsFile, totalFileSize, true);
            /*
                LOGD << io::xprintf("New file %s of the size %ld bytes was created.",
                                    projectionsFile.c_str(), totalFileSize);
            */
        }
        uint8_t buf[18];
        if(extended)
        {
            util::putUint16(0, &buf[0]);
            util::putUint16(0, &buf[2]);
            util::putUint16(0, &buf[4]);
            util::putUint32(dimy, &buf[0]);
            util::putUint32(dimx, &buf[2]);
            util::putUint32(dimz, &buf[4]);
            io::writeFirstBytes(projectionsFile, buf, 18);
        } else
        {
            util::putUint16((uint16_t)dimy, &buf[0]);
            util::putUint16((uint16_t)dimx, &buf[2]);
            util::putUint16((uint16_t)dimz, &buf[4]);
            io::writeFirstBytes(projectionsFile, buf, 6);
        }
        buffer = new uint8_t[sizeof(T) * this->dimx() * this->dimy()];
    }

    /// Guard for move constructor stealed objects
    template <typename T>
    DenAsyncFrame2DWritter<T>::~DenAsyncFrame2DWritter()
    {
        if(buffer != nullptr)
            delete[] buffer;
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
    DenAsyncFrame2DWritter<T>& DenAsyncFrame2DWritter<T>::
    operator=(const DenAsyncFrame2DWritter<T>& b)
    {
        LOGD << "Caling Copy assignment constructor of DenAsyncFrame2DWritter";
        if(&b != this) // To elegantly solve situation when assigning to itself
        {
            this->projectionsFile = b.projectionsFile;
            this->sizex = b.dimx;
            this->sizey = b.dimy;
            this->sizez = b.dimz;
            this->offset = offset;
            this->extended = extended;
            if(this->buffer != nullptr)
            {
                delete[] this->buffer;
                this->buffer = nullptr;
            }
            this->buffer = new uint8_t[sizeof(T) * this->dimx() * this->dimy()];
        }
    }

    // Move constructor
    template <typename T>
    DenAsyncFrame2DWritter<T>::DenAsyncFrame2DWritter(DenAsyncFrame2DWritter<T>&& b)
    {
        LOGD << "Caling Move constructor of DenAsyncFrame2DWritter";
        this->projectionsFile = b.projectionsFile;
        this->sizex = b.dimx;
        this->sizey = b.dimy;
        this->sizez = b.dimz;
        this->offset = b.offset;
        this->extended = b.extended;
        this->buffer = b.buffer;
        b.buffer = nullptr;
    }

    // Move assignment
    template <typename T>
    DenAsyncFrame2DWritter<T>& DenAsyncFrame2DWritter<T>::operator=(DenAsyncFrame2DWritter<T>&& b)
    {
        LOGD << "Caling Move assignment constructor of DenAsyncFrame2DWritter";
        if(&b != this) // To elegantly solve situation when assigning to itself
        {
            this->projectionsFile = b.projectionsFile;
            this->sizex = b.dimx;
            this->sizey = b.dimy;
            this->sizez = b.dimz;
            this->offset = b.offset;
            this->extended = b.extended;
            if(this->buffer != nullptr)
            {
                delete[] this->buffer;
                this->buffer = nullptr;
            }
            this->buffer = b.buffer;
            b.buffer = nullptr;
        }
    }

    template <typename T>
    std::string DenAsyncFrame2DWritter<T>::getFileName() const
    {
        return this->projectionsFile;
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
                util::setNextElement<T>(s(k, j), &buffer[(j * this->dimx() + k) * sizeof(T)]);
            }
        }
        uint64_t position = offset + ((uint64_t)sizeof(T)) * i * this->dimx() * this->dimy();
        io::writeBytesFrom(projectionsFile, position, buffer,
                           sizeof(T) * this->dimx() * this->dimy());
        return;
    }

} // namespace io
} // namespace CTL

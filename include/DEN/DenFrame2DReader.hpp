#pragma once

// External
#include <mutex>
#include <string>

// Internal
#include "BufferedFrame2D.hpp"
#include "DEN/DenFileInfo.hpp"
#include "Frame2DI.hpp"
#include "Frame2DReaderI.hpp"

namespace CTL {
namespace io {
    /**
     * Implementation of the ProjectionReader for projections and projection matrices stored in the
     * den files.
     */
    template <typename T>
    class DenFrame2DReader : virtual public Frame2DReaderI<T>
    // Frame2DReaderI<T> will be only once in the family tree
    {
    protected: // Visible in inheritance structure
        int sizex, sizey, sizez;
        std::string denFile;
        DenSupportedType dataType;
        int elementByteSize;
        uint8_t* buffer;
        T* buffer_copy;

    public:
        /**Constructs DenFrame2DReader from file name.
         *
         *@param denFile File in a DEN format that is about to read by frames..
         */
        DenFrame2DReader(std::string denFile);
        /// Destructor
        ~DenFrame2DReader();
        /// Copy constructor
        DenFrame2DReader(const DenFrame2DReader<T>& b);
        // Copy assignment
        DenFrame2DReader<T>& operator=(const DenFrame2DReader<T>& b);
        // Move constructor
        DenFrame2DReader(DenFrame2DReader<T>&& b);
        // Move assignment
        DenFrame2DReader<T>& operator=(DenFrame2DReader<T>&& other);
        std::shared_ptr<io::Frame2DI<T>> readFrame(unsigned int i) override;
        std::shared_ptr<io::BufferedFrame2D<T>> readBufferedFrame(unsigned int i);
        unsigned int dimx() const override;
        unsigned int dimy() const override;
        unsigned int dimz() const override;
        std::string getFileName() const;
        /**Returns file name of the underlying DEN file.**/
    private:
        mutable std::mutex consistencyMutex;
    };

    template <typename T>
    DenFrame2DReader<T>::DenFrame2DReader(std::string denFile)
    {
        this->denFile = denFile;
        DenFileInfo pi = DenFileInfo(this->denFile);
        this->sizex = pi.dimx();
        this->sizey = pi.dimy();
        this->sizez = pi.dimz();
        this->dataType = pi.getDataType();
        this->elementByteSize = pi.elementByteSize();
        this->buffer = new uint8_t[elementByteSize * sizex * sizey];
        this->buffer_copy = new T[sizex * sizey];
        // Buffers are used for the alocation of new frames. Since this class uses the instance that
        // copies memory, this memory might me reused.
    }

    // This mumbo jumbo is for correctly deleting object after move assignment operation was
    // performed to be able to delete such object with stealed internals where its buffers are
    // nullptr.
    template <typename T>
    DenFrame2DReader<T>::~DenFrame2DReader()
    {
        if(buffer != nullptr)
        {
            delete[] buffer;
        }
        buffer = nullptr;
        if(buffer_copy != nullptr)
        {
            delete[] buffer_copy;
        }
        buffer_copy = nullptr;
    }

    /**Copy constructor of DenFrame2DReader from another element.
     *Basically contructs everything from scratch by probing file on the disk.
     */
    template <typename T>
    DenFrame2DReader<T>::DenFrame2DReader(const DenFrame2DReader<T>& b)
        : DenFrame2DReader<T>::DenFrame2DReader(b.denFile)
    {
    }

    /**Copy assignment
     *
     */
    template <typename T>
    DenFrame2DReader<T>& DenFrame2DReader<T>::operator=(const DenFrame2DReader<T>& b)
    {
        if(&b != this) // To elegantly solve situation when assigning to itself
        {
            this->denFile = b.denFile;
            this->sizey = b.sizey;
            this->sizex = b.sizex;
            this->sizez = b.sizez;
            this->dataType = b.dataType;
            this->elementByteSize = b.elementByteSize;
            if(this->buffer != nullptr)
            {
                delete[] this->buffer;
            }
            this->buffer = nullptr;
            if(this->buffer_copy != nullptr)
            {
                delete[] this->buffer_copy;
            }
            this->buffer_copy = nullptr;
            this.buffer = new uint8_t[elementByteSize * sizex * sizey];
            this.buffer_copy = new T[sizex * sizey];
            // No memcpy due to the temporary buffer usage.
        }
        return *this;
    }

    template <typename T>
    DenFrame2DReader<T>::DenFrame2DReader(DenFrame2DReader<T>&& b)
    {
        this->denFile = b.denFile;
        this->sizex = b.sizex;
        this->sizey = b.sizey;
        this->sizez = b.sizez;
        this->dataType = b.dataType;
        this->elementByteSize = b.elementByteSize;
        this->buffer = b.buffer;
        b.buffer = nullptr;
        this->buffer_copy = b.buffer_copy;
        b.buffer_copy = nullptr;
    } // Move constructor to steal resources from another object

    template <typename T>
    DenFrame2DReader<T>& DenFrame2DReader<T>::operator=(DenFrame2DReader<T>&& other)
    {
        if(&other != this) // To elegantly solve situation when assigning to itself
        {
            if(this->buffer != nullptr)
            {
                delete[] this->buffer;
            }
            this->buffer = other.buffer;
            other.buffer = nullptr;
            if(this->buffer_copy != nullptr)
            {
                delete[] this->buffer_copy;
            }
            this->buffer_copy = other.buffer_copy;
            other.buffer_copy = nullptr;
            this->denFile = other.denFile;
            this->sizex = other.sizex;
            this->sizey = other.sizey;
            this->sizez = other.sizez;
            this->dataType = other.dataType;
            this->elementByteSize = other.elementByteSize;
        }
        return *this;
    } // Move assignment

    template <typename T>
    std::string DenFrame2DReader<T>::getFileName() const
    {
        return this->denFile;
    }

    template <typename T>
    unsigned int DenFrame2DReader<T>::dimx() const
    {
        return sizex;
    }

    template <typename T>
    unsigned int DenFrame2DReader<T>::dimy() const
    {
        return sizey;
    }

    template <typename T>
    unsigned int DenFrame2DReader<T>::dimz() const
    {
        return sizez;
    }

    template <typename T>
    std::shared_ptr<io::Frame2DI<T>> DenFrame2DReader<T>::readFrame(unsigned int sliceNum)
    {
        std::shared_ptr<Frame2DI<T>> ps = readBufferedFrame(sliceNum);
        return ps;
    }

    template <typename T>
    std::shared_ptr<io::BufferedFrame2D<T>>
    DenFrame2DReader<T>::readBufferedFrame(unsigned int sliceNum)
    {
        std::lock_guard<std::mutex> guard(consistencyMutex);
        // Mutex will be released as this goes out of scope.
        // To protect calling this method from another thread using the same block of memory
        uint64_t position = uint64_t(6) + uint64_t(sliceNum) * elementByteSize * sizex * sizey;
        io::readBytesFrom(this->denFile, position, buffer, elementByteSize * sizex * sizey);
        for(int a = 0; a != sizex * sizey; a++)
        {
            buffer_copy[a] = util::getNextElement<T>(&buffer[a * elementByteSize], dataType);
        }
        std::shared_ptr<BufferedFrame2D<T>> ps
            = std::make_shared<BufferedFrame2D<T>>(buffer_copy, sizex, sizey);
        return ps;
    }
} // namespace io
} // namespace CTL

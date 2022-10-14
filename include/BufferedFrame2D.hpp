#pragma once

// Internal
#include "Frame2DI.hpp"

namespace KCT {
namespace io {
    /**Implementation of Frame2DI in the underlying structure of naked array.
     *
     *T is a type of the data used, might be float, double or
     *uint16_t. This class is managed, the memory it manages fulfils rule of five, see
     *https://cpppatterns.com/patterns/rule-of-five.html
     */
    template <typename T>
    class BufferedFrame2D : public Frame2DI<T>
    {
    public:
        /**Create BufferedFrame2D using buffer of the data.
         *
         *Can be called with a buffer that will be used to construct the frame. Deep copy is created
         *while creating object. When the buffer is nullptr, memory is allocated but nothing is
         *copied. Frame then have to be filled by calling set to individual elements.
         */
        BufferedFrame2D(T* buffer, uint32_t sizex, uint32_t sizey)
            : sizex(sizex)
            , sizey(sizey)
            , frameSize((uint64_t)sizex * (uint64_t)sizey)
        {
            this->frameDataArray = new T[frameSize];
            if(buffer != nullptr)
            {
                memcpy(this->frameDataArray, buffer, frameSize * sizeof(T));
            }
        }

        /**Create BufferedFrame2D using single value to be filled into the buffer.
         *
         *This constructor allocates memory and fills it with a certain value that will be used for
         *the whole buffer.
         */
        BufferedFrame2D(T elm, uint32_t sizex, uint32_t sizey)
            : sizex(sizex)
            , sizey(sizey)
            , frameSize((uint64_t)sizex * (uint64_t)sizey)

        {
            this->frameDataArray = new T[frameSize];
            std::fill(this->frameDataArray, this->frameDataArray + frameSize, elm);
        }

        /**Copy constructor of BufferedFrame2D from another element.
         *
         */
        BufferedFrame2D(const BufferedFrame2D& b)
            : BufferedFrame2D(b.frameDataArray, b.sizex, b.sizey)
        {
        }

        /**Destructor of BufferedFrame2D.
         *
         */
        ~BufferedFrame2D()
        {
            if(frameDataArray != nullptr)
            {
                delete[] frameDataArray;
            }
            frameDataArray = nullptr;
        } // destructor

        /**Copy assignment
         *
         */
        BufferedFrame2D& operator=(const BufferedFrame2D& b)
        {
            if(&b != this)
            {
                this->sizex = b.sizex;
                this->sizey = b.sizey;
                this->frameSize = b.frameSize;
                if(this->frameDataArray != nullptr)
                {
                    delete[] this->frameDataArray;
                }
                this->frameDataArray = nullptr;
                this->frameDataArray = new T[frameSize];
                memcpy(this->frameDataArray, b.frameDataArray, frameSize * sizeof(T));
            }
            return *this;

        } // copy assignment, tmp is to solve situation when assigning to itself

        BufferedFrame2D(BufferedFrame2D&& other)
        {
            this->frameDataArray = other.frameDataArray;
            this->sizex = other.sizex;
            this->sizey = other.sizey;
            this->frameSize = other.frameSize;
            other.frameDataArray = nullptr;
        } // Move constructor

        BufferedFrame2D& operator=(BufferedFrame2D&& other)
        {
            if(&other != this)
            {
                if(this->frameDataArray != nullptr)
                {
                    delete[] this->frameDataArray;
                }
                this->frameDataArray = nullptr;
                this->frameDataArray = other.frameDataArray;
                this->sizex = other.sizex;
                this->sizey = other.sizey;
                this->frameSize = other.frameSize;
                other.frameDataArray = nullptr;
            }
            return *this;
        } // Move assignment

        T get(uint32_t x, uint32_t y) const override { return frameDataArray[y * sizex + x]; }

        void set(T val, uint32_t x, uint32_t y) override { frameDataArray[y * sizex + x] = val; }

        uint32_t dimx() const override { return sizex; }

        uint32_t dimy() const override { return sizey; }

        uint64_t getFrameSize() const override { return frameSize; }

        /**Return transposed frame as a new object
         *
         *The object itself is not modified.
         */
        std::shared_ptr<io::BufferedFrame2D<T>> transposed()
        {
            std::shared_ptr<io::BufferedFrame2D<T>> ft
                = std::make_shared<BufferedFrame2D<T>>(nullptr, sizey, sizex);
            for(uint32_t x = 0; x != sizex; x++)
                for(uint32_t y = 0; y != sizey; y++)
                {
                    ft->set(frameDataArray[y * sizex + x], y, x);
                }
            return ft;
        }

        /**
         *Function to get access to the data array.
         *
         *Potentially unsafe operation since it is structure managed by the object.
         *This array is destroyed after the object is destroyed.
         */
        T* getDataPointer() { return frameDataArray; }

    private:
        T* frameDataArray;
        const uint32_t sizex, sizey;
        const uint64_t frameSize;
    };
} // namespace io
} // namespace KCT

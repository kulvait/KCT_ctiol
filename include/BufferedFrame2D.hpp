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
        {
            this->slice = new T[sizex * sizey];
            if(buffer != nullptr)
            {
                memcpy(this->slice, buffer, sizex * sizey * sizeof(T));
            }
            this->sizex = sizex;
            this->sizey = sizey;
        }

        /**Create BufferedFrame2D using single value to be filled into the buffer.
         *
         *This constructor allocates memory and fills it with a certain value that will be used for
         *the whole buffer.
         */
        BufferedFrame2D(T elm, uint32_t sizex, uint32_t sizey)
        {
            this->slice = new T[sizex * sizey];
            std::fill(this->slice, this->slice + (sizex * sizey), elm);
            this->sizex = sizex;
            this->sizey = sizey;
        }

        /**Copy constructor of BufferedFrame2D from another element.
         *
         */
        BufferedFrame2D(const BufferedFrame2D& b)
            : BufferedFrame2D(b.slice, b.sizex, b.sizey)
        {
        }

        /**Destructor of BufferedFrame2D.
         *
         */
        ~BufferedFrame2D()
        {
            if(slice != nullptr)
            {
                delete[] slice;
            }
            slice = nullptr;
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
                if(this->slice != nullptr)
                {
                    delete[] this->slice;
                }
                this->slice = nullptr;
                this->slice = new T[sizex * sizey];
                memcpy(this->slice, b.slice, sizex * sizey * sizeof(T));
            }
            return *this;

        } // copy assignment, tmp is to solve situation when assigning to itself

        BufferedFrame2D(BufferedFrame2D&& other)
        {
            this->slice = other.slice;
            this->sizex = other.sizex;
            this->sizey = other.sizey;
            other.slice = nullptr;
        } // Move constructor

        BufferedFrame2D& operator=(BufferedFrame2D&& other)
        {
            if(&other != this)
            {
                if(this.slice != nullptr)
                {
                    delete[] this->slice;
                }
                this->slice = nullptr;
                this->slice = other.slice;
                this->sizex = other.sizex;
                this->sizey = other.sizey;
                other.slice = nullptr;
            }
            return *this;
        } // Move assignment

        T get(unsigned int x, unsigned int y) const override { return slice[y * sizex + x]; }

        void set(T val, unsigned int x, unsigned int y) override { slice[y * sizex + x] = val; }

        unsigned int dimx() const override { return sizex; }

        unsigned int dimy() const override { return sizey; }

        /**Return transposed frame as a new object
         *
         *The object itself is not modified.
         */
        std::shared_ptr<io::BufferedFrame2D<T>> transposed()
        {
            std::shared_ptr<io::BufferedFrame2D<T>> ft
                = std::make_shared<BufferedFrame2D<T>>(nullptr, sizey, sizex);
            for(int x = 0; x != sizex; x++)
                for(int y = 0; y != sizey; y++)
                {
                    ft->set(slice[y * sizex + x], y, x);
                }
            return ft;
        }

        /**
         *Function to get access to the data array.
         *
         *Dangerous operation since it is structure managed by the object.
         *This array is destroyed after the object is destroyed.
         */
        T* getDataPointer() { return slice; }

    private:
        T* slice;
        int sizex, sizey;
    };
} // namespace io
} // namespace KCT

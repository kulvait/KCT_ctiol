#pragma once
// Logging
#include <plog/Log.h>

// External dependencies
#include <cstdint>
#include <cstring>
#include <string>

#include "PROG/KCTException.hpp"

namespace KCT {
namespace util {

    uint8_t nextUint8(uint8_t* buffer);
    int16_t nextInt8(uint8_t* buffer);
    uint16_t nextUint16(uint8_t* buffer);
    int16_t nextInt16(uint8_t* buffer);
    uint32_t nextUint32(uint8_t* buffer);
    int32_t nextInt32(uint8_t* buffer);
    uint64_t nextUint64(uint8_t* buffer);
    int64_t nextInt64(uint8_t* buffer);
    float nextFloat(uint8_t* buffer);
    double nextDouble(uint8_t* buffer);

    void putUint8(uint8_t val, uint8_t* buffer);
    void putInt8(int16_t val, uint8_t* buffer);
    void putUint16(uint16_t val, uint8_t* buffer);
    void putInt16(int16_t val, uint8_t* buffer);
    void putUint32(uint32_t val, uint8_t* buffer);
    void putInt32(int32_t val, uint8_t* buffer);
    void putUint64(uint64_t val, uint8_t* buffer);
    void putInt64(int64_t val, uint8_t* buffer);
    void putFloat(float val, uint8_t* buffer);
    void putDouble(double val, uint8_t* buffer);

    // Read about template specialization
    // https://en.cppreference.com/w/cpp/language/template_specialization
    template <typename T>
    void setNextElement(T val, uint8_t* buffer)
    {
        KCTERR("There is no specialized template for handling given type!");
    }

    // See
    // https://stackoverflow.com/questions/4445654/multiple-definition-of-template-specialization-when-using-different-objects
    template <>
    inline void setNextElement<uint16_t>(uint16_t val, uint8_t* buffer)
    {
        putUint16(val, buffer);
    }

    template <>
    inline void setNextElement<int16_t>(int16_t val, uint8_t* buffer)
    {
        putInt16(val, buffer);
    }

    template <>
    inline void setNextElement<uint32_t>(uint32_t val, uint8_t* buffer)
    {
        putUint32(val, buffer);
    }

    template <>
    inline void setNextElement<int32_t>(int32_t val, uint8_t* buffer)
    {
        putInt32(val, buffer);
    }

    template <>
    inline void setNextElement<uint64_t>(uint64_t val, uint8_t* buffer)
    {
        putUint64(val, buffer);
    }

    template <>
    inline void setNextElement<int64_t>(int64_t val, uint8_t* buffer)
    {
        putInt64(val, buffer);
    }

    template <>
    inline void setNextElement<float>(float val, uint8_t* buffer)
    {
        putFloat(val, buffer);
    }

    template <>
    inline void setNextElement<double>(double val, uint8_t* buffer)
    {
        putDouble(val, buffer);
    }

    template <>
    inline void setNextElement<uint8_t>(uint8_t val, uint8_t* buffer)
    {
        putUint8(val, buffer);
    }

} // namespace util
} // namespace KCT

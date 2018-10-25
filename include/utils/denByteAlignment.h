#pragma once
// Logging
#include <plog/Log.h>

// External dependencies
#include <cstdint>
#include <cstring>
#include <string>

// Internal dependencies
#include "DEN/DenSupportedType.hpp"
#include "stringFormatter.h"

namespace CTL {
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

    template <typename T>
    T getNextElement(uint8_t* buffer, io::DenSupportedType dataType)
    {
        switch(dataType)
        {
        case io::DenSupportedType::uint16_t_:
            return nextUint16(buffer);
        case io::DenSupportedType::float_:
            return nextFloat(buffer);
        case io::DenSupportedType::double_:
            return nextDouble(buffer);
        default:
            std::string errMsg
                = io::xprintf("Unsupported data type %s.", io::DenSupportedTypeToString(dataType));
            LOGE << errMsg;
            throw std::runtime_error(errMsg);
        }
    }

    template <typename T>
    void setNextElement(T val, uint8_t* buffer)
    {
        switch(sizeof(T))
        {
        case 2:
            putUint16(val, buffer);
            break;
        case 4:
            putFloat(val, buffer);
            break;
        case 8:
            putDouble(val, buffer);
            break;
        }
    }

} // namespace util
} // namespace CTL

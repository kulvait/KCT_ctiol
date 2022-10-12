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

    uint8_t inline nextUint8(uint8_t* buffer);
    int16_t inline nextInt8(uint8_t* buffer);
    uint16_t inline nextUint16(uint8_t* buffer);
    int16_t inline nextInt16(uint8_t* buffer);
    uint32_t inline nextUint32(uint8_t* buffer);
    int32_t inline nextInt32(uint8_t* buffer);
    uint64_t inline nextUint64(uint8_t* buffer);
    int64_t inline nextInt64(uint8_t* buffer);
    float inline nextFloat(uint8_t* buffer);
    double inline nextDouble(uint8_t* buffer);

    void inline putUint8(const uint8_t& val, uint8_t* buffer);
    void inline putInt8(const int16_t& val, uint8_t* buffer);
    void inline putUint16(const uint16_t& val, uint8_t* buffer);
    void inline putInt16(const int16_t& val, uint8_t* buffer);
    void inline putUint32(const uint32_t& val, uint8_t* buffer);
    void inline putInt32(const int32_t& val, uint8_t* buffer);
    void inline putUint64(const uint64_t& val, uint8_t* buffer);
    void inline putInt64(const int64_t& val, uint8_t* buffer);
    void inline putFloat(const float& val, uint8_t* buffer);
    void inline putDouble(const double& val, uint8_t* buffer);

    // Read about template specialization
    // https://en.cppreference.com/w/cpp/language/template_specialization
    template <typename T>
    void setNextElement(const T& val, uint8_t* buffer) = delete;
    /*
        {
            KCTERR("There is no specialized template for handling given type!");
        }
    */
    // See
    // https://stackoverflow.com/questions/4445654/multiple-definition-of-template-specialization-when-using-different-objects
    template <>
    inline void setNextElement<uint16_t>(const uint16_t& val, uint8_t* buffer)
    {
        putUint16(val, buffer);
    }

    template <>
    inline void setNextElement<int16_t>(const int16_t& val, uint8_t* buffer)
    {
        putInt16(val, buffer);
    }

    template <>
    inline void setNextElement<uint32_t>(const uint32_t& val, uint8_t* buffer)
    {
        putUint32(val, buffer);
    }

    template <>
    inline void setNextElement<int32_t>(const int32_t& val, uint8_t* buffer)
    {
        putInt32(val, buffer);
    }

    template <>
    inline void setNextElement<uint64_t>(const uint64_t& val, uint8_t* buffer)
    {
        putUint64(val, buffer);
    }

    template <>
    inline void setNextElement<int64_t>(const int64_t& val, uint8_t* buffer)
    {
        putInt64(val, buffer);
    }

    template <>
    inline void setNextElement<float>(const float& val, uint8_t* buffer)
    {
        putFloat(val, buffer);
    }

    template <>
    inline void setNextElement<double>(const double& val, uint8_t* buffer)
    {
        putDouble(val, buffer);
    }

    template <>
    inline void setNextElement<uint8_t>(const uint8_t& val, uint8_t* buffer)
    {
        putUint8(val, buffer);
    }

    // Definitions of the inlines declared before
    //
    // I decided to put data into the char* array instead of uint8_t* for the systems where byte
    // size is not 8bit, where reading should work bite wise and alignment considerations raised by
    // https://stackoverflow.com/questions/39668561/allocate-n-bytes-by-new-and-fill-it-with-any-type?utm_medium=organic&utm_source=google_rich_qa&utm_campaign=google_rich_qa
    uint8_t inline nextUint8(uint8_t* buffer)
    {
        uint8_t num;
        num = buffer[0];
        return (num);
    }

    int16_t inline nextInt8(uint8_t* buffer) { return (int8_t)nextUint8(buffer); }

    uint16_t nextUint16(uint8_t* buffer)
    {
        uint16_t num;
        num = buffer[0] | buffer[1] << 8;
        return (num);
    }

    int16_t inline nextInt16(uint8_t* buffer) { return (int16_t)nextUint16(buffer); }

    uint32_t inline nextUint32(uint8_t* buffer)
    {
        uint32_t num;
        num = buffer[0] | buffer[1] << 8 | buffer[2] << 16 | buffer[3] << 24;
        // num = buffer[0] | buffer[1] << 8 | buffer[2] << 16 | 0x7f << 24;
        // num = 0xff | 0xff << 8 | 0xff << 16 | 0x7f << 24;
        // num = 2147483647;
        return (num);
    }

    uint64_t inline nextUint64(uint8_t* buffer)
    {
        uint64_t num;
        num = (uint64_t)buffer[0] | (uint64_t)buffer[1] << 8 | (uint64_t)buffer[2] << 16
            | (uint64_t)buffer[3] << 24 | (uint64_t)buffer[4] << 32 | (uint64_t)buffer[5] << 40
            | (uint64_t)buffer[6] << 48 | (uint64_t)buffer[7] << 56;
        return (num);
    }

    int32_t inline nextInt32(uint8_t* buffer) { return (int32_t)nextUint32(buffer); }

    int64_t inline nextInt64(uint8_t* buffer) { return (int64_t)nextUint64(buffer); }

    /*This function might not preserve endianness.
     *
     */
    float inline nextFloat(uint8_t* buffer)
    {
        uint32_t out_int;
        out_int = nextUint32(buffer);
        float out;
        std::memcpy(&out, &out_int, 4);
        // simpler but do not preserve endianness memcpy(&out, buffer, 4);
        return out;
    }

    /*This function might not preserve endianness.
     *
     */
    double inline nextDouble(uint8_t* buffer)
    {
        uint64_t out_int;
        out_int = nextUint64(buffer);
        double out;
        std::memcpy(&out, &out_int, 8);
        // simpler but do not preserve endianness memcpy(&out, buffer, 8);
        return out;
    }

    void inline putUint8(const uint8_t& val, uint8_t* buffer) { buffer[0] = val; }

    void inline putInt8(const int16_t& val, uint8_t* buffer)
    {
        // First convert to unsigned type and then write into the buffer
        putUint8((uint8_t)val, buffer);
    }

    void inline putUint16(const uint16_t& val, uint8_t* buffer)
    {
        buffer[0] = val & 0xFF;
        buffer[1] = (val & 0xFF00) >> 8;
    }

    void inline putInt16(const int16_t& val, uint8_t* buffer) { putUint16((uint16_t)val, buffer); }

    void inline putUint32(const uint32_t& val, uint8_t* buffer)
    {
        buffer[0] = val & 0xFF;
        buffer[1] = (val & 0xFF00) >> 8;
        buffer[2] = (val & 0xFF0000) >> 16;
        buffer[3] = (val & 0xFF000000) >> 24;
    }

    void inline putInt32(const int32_t& val, uint8_t* buffer) { putUint32((uint32_t)val, buffer); }

    void inline putUint64(const uint64_t& val, uint8_t* buffer)
    {
        buffer[0] = val & 0xFF;
        buffer[1] = (val & 0xFF00) >> 8;
        buffer[2] = (val & 0xFF0000) >> 16;
        buffer[3] = (val & 0xFF000000) >> 24;
        buffer[4] = (val & 0xFF00000000) >> 32;
        buffer[5] = (val & 0xFF0000000000) >> 40;
        buffer[6] = (val & 0xFF000000000000) >> 48;
        buffer[7] = (val & 0xFF00000000000000) >> 56;
    }
    void inline putInt64(const int64_t& val, uint8_t* buffer) { putUint64((uint64_t)val, buffer); }

    void inline putFloat(const float& val, uint8_t* buffer)
    {
        uint32_t tmp;
        std::memcpy(&tmp, &val, 4);
        putUint32(tmp, buffer);
    }

    void inline putDouble(const double& val, uint8_t* buffer)
    {
        uint64_t tmp;
        std::memcpy(&tmp, &val, 8);
        putUint64(tmp, buffer);
    }

} // namespace util
} // namespace KCT

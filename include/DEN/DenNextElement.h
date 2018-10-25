#pragma once
// Logging
#include <plog/Log.h>

// External dependencies
#include <cstdint>
#include <cstring>
#include <string>

// Internal dependencies
#include "DEN/DenSupportedType.hpp"
#include "littleEndianAlignment.h"
#include "stringFormatter.h"

namespace CTL {
namespace util {

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
} // namespace util
} // namespace CTL

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

namespace KCT::util {

template <typename T>
T getNextElement(uint8_t* buffer, io::DenSupportedType dataType)
{
    switch(dataType)
    {
    case io::DenSupportedType::UINT16:
        return static_cast<T>(nextUint16(buffer));
    case io::DenSupportedType::FLOAT32:
        return static_cast<T>(nextFloat(buffer));
    case io::DenSupportedType::FLOAT64:
        return static_cast<T>(nextDouble(buffer));
    default:
        std::string errMsg = io::xprintf("Unsupported data type %s.",
                                         io::DenSupportedTypeToString(dataType).c_str());
        KCTERR(errMsg);
    }
}
} // namespace KCT::util

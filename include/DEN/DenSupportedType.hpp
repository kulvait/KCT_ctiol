#pragma once

#include <string>

#include "stringFormatter.h"

namespace CTL {
namespace io {

    enum DenSupportedType { uint16_t_, float_, double_ };

    inline std::string DenSupportedTypeToString(DenSupportedType dataType)
    {
        switch(dataType)
        {
        case uint16_t_:
            return "uint16_t";
        case float_:
            return "float";
        case double_:
            return "double";
        default:
            return "[Unknown DenSupportedType]";
        }
    }

    inline DenSupportedType getSupportedTypeByByteLength(int byteLength)
    {
        switch(byteLength)
        {
        case sizeof(uint16_t):
            return DenSupportedType::uint16_t_;
        case sizeof(float):
            return DenSupportedType::float_;
        case sizeof(double):
            return DenSupportedType::double_;
        default:
            io::throwerr("Unknown DenSupportedType of the length %d", byteLength);
        }
    }

} // namespace io
} // namespace CTL

#pragma once

#include <string>

#include "PROG/KCTException.hpp"

namespace KCT::io {

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

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wreturn-type"
inline DenSupportedType getSupportedTypeByByteLength(uint32_t byteLength)
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
        KCTERR(io::xprintf("Unknown DenSupportedType of the length %d", byteLength));
    }
}
#pragma GCC diagnostic pop

} // namespace KCT::io

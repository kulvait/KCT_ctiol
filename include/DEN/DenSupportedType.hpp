#pragma once

#include <string>
#include <typeinfo>

#include "PROG/KCTException.hpp"

namespace KCT::io {

enum DenSupportedType { UINT16, INT16, UINT32, INT32, UINT64, INT64, FLOAT32, FLOAT64, UINT8 };

inline std::string DenSupportedTypeToString(DenSupportedType dataType)
{
    switch(dataType)
    {
    case UINT16:
        return "UINT16";
    case INT16:
        return "INT16";
    case UINT32:
        return "UINT32";
    case INT32:
        return "INT32";
    case UINT64:
        return "UINT64";
    case INT64:
        return "INT64";
    case FLOAT32:
        return "FLOAT32";
    case FLOAT64:
        return "FLOAT64";
    case UINT8:
        return "UINT8";
    default:
        return "[Unknown DenSupportedType]";
    }
}

inline uint16_t DenSupportedTypeID(DenSupportedType dataType)
{
    switch(dataType)
    {
    case UINT16:
        return 0;
    case INT16:
        return 1;
    case UINT32:
        return 2;
    case INT32:
        return 3;
    case UINT64:
        return 4;
    case INT64:
        return 5;
    case FLOAT32:
        return 6;
    case FLOAT64:
        return 7;
    case UINT8:
        return 8;
    default:
        KCTERR("[Unknown DenSupportedType]");
    }
}

inline uint16_t DenSupportedTypeElementByteSize(DenSupportedType dataType)
{
    switch(dataType)
    {
    case UINT16:
        return 2;
    case INT16:
        return 2;
    case UINT32:
        return 4;
    case INT32:
        return 4;
    case UINT64:
        return 8;
    case INT64:
        return 8;
    case FLOAT32:
        return 4;
    case FLOAT64:
        return 8;
    case UINT8:
        return 1;
    default:
        KCTERR("[Unknown DenSupportedType]");
    }
}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wreturn-type"
inline DenSupportedType getLegacySupportedTypeByByteLength(uint32_t byteLength)
{
    switch(byteLength)
    {
    case 2:
        return DenSupportedType::UINT16;
    case 4:
        return DenSupportedType::FLOAT32;
    case 8:
        return DenSupportedType::FLOAT64;
    default:
        KCTERR(io::xprintf("Unknown DenSupportedType of the length %d", byteLength));
    }
}

inline DenSupportedType getDenSupportedTypeByID(uint32_t ID)
{
    switch(ID)
    {
    case 0:
        return DenSupportedType::UINT16;
    case 1:
        return DenSupportedType::INT16;
    case 2:
        return DenSupportedType::UINT32;
    case 3:
        return DenSupportedType::INT32;
    case 4:
        return DenSupportedType::UINT64;
    case 5:
        return DenSupportedType::INT64;
    case 6:
        return DenSupportedType::FLOAT32;
    case 7:
        return DenSupportedType::FLOAT64;
    case 8:
        return DenSupportedType::UINT8;
    default:
        KCTERR(io::xprintf("[Unknown DenSupportedType with ID %d]", ID));
    }
}

inline DenSupportedType getDenSupportedTypeByTypeID(const std::type_info& inf)
{
    if(inf == typeid(uint16_t))
    {
        return DenSupportedType::UINT16;
    } else if(inf == typeid(int16_t))
    {
        return DenSupportedType::INT16;
    } else if(inf == typeid(uint32_t))
    {
        return DenSupportedType::UINT32;
    } else if(inf == typeid(int32_t))
    {
        return DenSupportedType::INT32;
    } else if(inf == typeid(uint64_t))
    {
        return DenSupportedType::UINT64;
    } else if(inf == typeid(int64_t))
    {
        return DenSupportedType::INT64;
    } else if(inf == typeid(float))
    {
        return DenSupportedType::FLOAT32;
    } else if(inf == typeid(double))
    {
        return DenSupportedType::FLOAT64;
    } else if(inf == typeid(uint8_t))
    {
        return DenSupportedType::UINT8;
    } else
    {
        KCTERR(io::xprintf("[Unknown DenSupportedType with type %s]", inf.name()));
    }
}
#pragma GCC diagnostic pop

} // namespace KCT::io

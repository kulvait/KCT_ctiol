#pragma once
#include <cstdarg> //va_start, va_end
#include <cstring> //strcpy
#include <memory> //unique_ptr
#include <plog/Log.h> //LOGE for throwerr

namespace CTL {
namespace io {
    std::string xprintf(const std::string fmt_str, ...);
    void throwerr(const std::string fmt_str, ...);
} // namespace io
} // namespace CTL

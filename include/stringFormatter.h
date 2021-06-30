#pragma once
#include <cstdarg> //va_start, va_end
#include <cstring> //strcpy
#include <memory> //unique_ptr
#include <plog/Log.h> //LOGE for throwerr

#ifndef COLORCODES
#define COLORCODES
#define RESET "\033[0m"
#define BLACK "\033[30m" /* Black */
#define RED "\033[31m" /* Red */
#define GREEN "\033[32m" /* Green */
#define YELLOW "\033[33m" /* Yellow */
#define BLUE "\033[34m" /* Blue */
#define MAGENTA "\033[35m" /* Magenta */
#define CYAN "\033[36m" /* Cyan */
#define WHITE "\033[37m" /* White */
#define BOLDBLACK "\033[1m\033[30m" /* Bold Black */
#define BOLDRED "\033[1m\033[31m" /* Bold Red */
#define BOLDGREEN "\033[1m\033[32m" /* Bold Green */
#define BOLDYELLOW "\033[1m\033[33m" /* Bold Yellow */
#define BOLDBLUE "\033[1m\033[34m" /* Bold Blue */
#define BOLDMAGENTA "\033[1m\033[35m" /* Bold Magenta */
#define BOLDCYAN "\033[1m\033[36m" /* Bold Cyan */
#define BOLDWHITE "\033[1m\033[37m" /* Bold White */
#endif

namespace CTL {
namespace io {
    std::string xprintf(const std::string fmt_str, ...);
    template <typename... Args>
    std::string xprintf_red(const std::string fmt_str, Args... args)
    {   
        return xprintf(RED + fmt_str + RESET, args...);
    }   
    template <typename... Args>
    std::string xprintf_green(const std::string fmt_str, Args... args)
    {   
        return xprintf(GREEN + fmt_str + RESET, args...);
    }   
    template <typename... Args>
    std::string xprintf_blue(const std::string fmt_str, Args... args)
    {   
        return xprintf(BLUE + fmt_str + RESET, args...);
    }   
    template <typename... Args>
    std::string xprintf_yellow(const std::string fmt_str, Args... args)
    {   
        return xprintf(YELLOW + fmt_str + RESET, args...);
    }   
    template <typename... Args>
    std::string xprintf_magenta(const std::string fmt_str, Args... args)
    {   
        return xprintf(MAGENTA + fmt_str + RESET, args...);
    }   
    void throwerr(const std::string fmt_str, ...);
} // namespace io
} // namespace CTL

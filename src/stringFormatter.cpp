#include "stringFormatter.h"

namespace CTL {
namespace io {

    std::string xprintf_red(const std::string fmt_str, ...)
    {
        va_list ap;
        va_start(ap, fmt_str);
        std::string x =  xprintf_va(RED + fmt_str + RESET, ap);
        va_end(ap);
        return x;
    }
    std::string xprintf_green(const std::string fmt_str, ...)
    {
        va_list ap;
        va_start(ap, fmt_str);
        std::string x =  xprintf_va(GREEN + fmt_str + RESET, ap);
        va_end(ap);
        return x;
    }
    std::string xprintf_blue(const std::string fmt_str, ...)
    {
        va_list ap;
        va_start(ap, fmt_str);
        std::string x =  xprintf_va(BLUE + fmt_str + RESET, ap);
        va_end(ap);
        return x;
    }
    std::string xprintf_yellow(const std::string fmt_str, ...)
    {
        va_list ap;
        va_start(ap, fmt_str);
        std::string x =  xprintf_va(YELLOW + fmt_str + RESET, ap);
        va_end(ap);
        return x;
    }
    std::string xprintf_magenta(const std::string fmt_str, ...)
    {
        va_list ap;
        va_start(ap, fmt_str);
        std::string x =  xprintf_va(MAGENTA + fmt_str + RESET, ap);
        va_end(ap);
        return x;
    }
    /**Formats string in a printf way and returns std::string as a result.
     */
    std::string xprintf(const std::string fmt_str, ...)
    {
        va_list ap;
        va_start(ap, fmt_str);
        std::string x = xprintf_va(fmt_str, ap);
        va_end(ap);
        return x;
    }

    std::string xprintf_va(const std::string fmt_str, va_list ap)
    {

        int final_n,
            n = ((int)fmt_str.size())
            * 2; /* Reserve two times as much as the length of the fmt_str */
        std::unique_ptr<char[]> formatted;
        while(1)
        {
            formatted.reset(new char[n]); /* Wrap the plain char array into the unique_ptr */
            strcpy(&formatted[0], fmt_str.c_str());
            final_n = vsnprintf(&formatted[0], n, fmt_str.c_str(), ap);
            if(final_n < 0 || final_n >= n)
                n += abs(final_n - n + 1);
            else
                break;
        }
        return std::string(formatted.get());
    }

    /**
     *This function formats string in a printf manner, writes it into LOGE and throws
     *std::runtime_error
     */
    void throwerr(const std::string fmt_str, ...)
    {
        std::string result;
        int final_n,
            n = ((int)fmt_str.size())
            * 2; /* Reserve two times as much as the length of the fmt_str */
        std::unique_ptr<char[]> formatted;
        va_list ap;
        while(1)
        {
            formatted.reset(new char[n]); /* Wrap the plain char array into the unique_ptr */
            strcpy(&formatted[0], fmt_str.c_str());
            va_start(ap, fmt_str);
            final_n = vsnprintf(&formatted[0], n, fmt_str.c_str(), ap);
            va_end(ap);
            if(final_n < 0 || final_n >= n)
                n += abs(final_n - n + 1);
            else
                break;
        }
        std::string msg = std::string(formatted.get());
        LOGE << msg;
        throw std::runtime_error(msg);
    }
} // namespace io
} // namespace CTL

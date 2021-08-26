#include "stringFormatter.h"

namespace KCT {
namespace io {
    /**Formats string in a printf way and returns std::string as a result.
     */
    std::string xprintf(const std::string fmt_str, ...)
    {
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
} // namespace KCT

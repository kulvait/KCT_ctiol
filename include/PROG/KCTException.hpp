#pragma once
#include <exception>
#include "stringFormatter.h"

// https://stackoverflow.com/questions/2849832/c-c-line-number
#define KCTERR(msg) throw KCT::util::CBCTException(msg, __FILE__, __LINE__, __func__);

namespace KCT::util {

class KCTException : public exception
{
    const char* file;
    int line;
    const char* function;
    const char* info;

public:
    KCTException(const char* msg, const char* file, int line, const char* function)
        : std::exception(msg)
        , file(file)
        , line(line)
        , function(function)
        , info(msg)
    {
    }

    const char* get_file() const { return file; }
    int get_line() const { return line; }
    const char* get_function() const { return function; }
    const char* get_info() const { return info; }
    const char* what() const throw()
    {
        return io::xprintf("%s:%d %s: Exception %s", file, line, function, info);
    }
};
} // namespace KCT::util

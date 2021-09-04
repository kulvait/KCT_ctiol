#pragma once
#include "stringFormatter.h"
#include <exception>

// https://stackoverflow.com/questions/2849832/c-c-line-number
#define KCTERR(msg) throw KCT::util::KCTException(msg, __FILE__, __LINE__, __func__);

namespace KCT::util {
using namespace std;

class KCTException : public exception
{
    const char* file;
    const int line;
    const char* function;
    const std::string msg;
    const std::string returnedMessage;

public:
    KCTException(std::string& msg, const char* file, int line, const char* function)
        : file(file)
        , line(line)
        , function(function)
        , msg(msg)
        , returnedMessage(
              io::xprintf("KCTException in [%s@%s:%d]: %s", function, file, line, msg.c_str()))
    {
    }

    const char* get_file() const { return file; }
    int get_line() const { return line; }
    const char* get_function() const { return function; }
    const std::string get_msg() const { return msg; }
    const char* what() const throw()
    {
        // Do not return c_str() of the string that gets destructed, ex returned from io::xprintf
        return returnedMessage.c_str();
    }
};
} // namespace KCT::util

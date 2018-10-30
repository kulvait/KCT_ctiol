#pragma once

#include <iomanip>
#include <plog/Record.h>
#include <plog/Util.h>

namespace plog {
/**This is modified plog::TxtFormatter to output also file name and line number.
 *
 */
class PlogFormatter
{
public:
    static util::nstring header()
    {
        return util::nstring();
    } ///< Method do the same as the same method of plog::TxtFormatter.

    /**Formats the output to include file name together with the line where the logging occurs.
     *
     */
    static util::nstring format(const Record& record)
    {
        tm t;
        util::localtime_s(&t, &record.getTime().time);

        util::nostringstream ss;
        ss << t.tm_year + 1900 << "-" << std::setfill(PLOG_NSTR('0')) << std::setw(2)
           << t.tm_mon + 1 << PLOG_NSTR("-") << std::setfill(PLOG_NSTR('0')) << std::setw(2)
           << t.tm_mday << PLOG_NSTR(" ");
        ss << std::setfill(PLOG_NSTR('0')) << std::setw(2) << t.tm_hour << PLOG_NSTR(":")
           << std::setfill(PLOG_NSTR('0')) << std::setw(2) << t.tm_min << PLOG_NSTR(":")
           << std::setfill(PLOG_NSTR('0')) << std::setw(2) << t.tm_sec << PLOG_NSTR(".")
           << std::setfill(PLOG_NSTR('0')) << std::setw(3) << record.getTime().millitm
           << PLOG_NSTR(" ");
        ss << std::setfill(PLOG_NSTR(' ')) << std::setw(5) << std::left
           << severityToString(record.getSeverity()) << PLOG_NSTR(" ");
        ss << PLOG_NSTR("[") << record.getTid() << PLOG_NSTR("] ");
        ss << PLOG_NSTR("[") << record.getFunc() << PLOG_NSTR("@") << record.getFile() << ":"
           << record.getLine() << PLOG_NSTR("] ");
        ss << record.getMessage() << PLOG_NSTR("\n");

        return ss.str();
    }
};

} // namespace plog

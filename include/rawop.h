#pragma once
// Raw reading of the part file into the uint8_t buffer

#include <plog/Log.h>

#include <fstream>
#include <iostream>
#include <limits.h>
#include <memory> // For std::unique_ptr
#include <sstream>
#include <stdarg.h> // For va_start, etc.
#include <string>
#include <sys/stat.h>

#include "littleEndianAlignment.h"

namespace CTL {
namespace io {
    void readFirstBytes(std::string fileName, uint8_t* buffer, int numBytes);
    void readBytesFrom(std::string fileName, uint64_t fromPosition, uint8_t* buffer, int numBytes);
    void writeFirstBytes(std::string fileName, uint8_t* buffer, int numBytes);
    void writeBytesFrom(std::string fileName, uint64_t fromPosition, uint8_t* buffer, int numBytes);
    void appendBytes(std::string fileName, uint8_t* buffer, int numBytes);
    void createEmptyFile(std::string fileName, uint64_t numBytes, bool overwrite);
    bool fileExists(std::string fileName);
    long getFileSize(std::string filename);
    std::string getParent(const std::string& path);
    std::string getBasename(const std::string& path);
} // namespace io
} // namespace CTL

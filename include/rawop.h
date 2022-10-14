#pragma once
// Raw reading of the part file into the uint8_t buffer

#include <plog/Log.h>

#include <cstdio>
#include <experimental/filesystem>
#include <fstream>
#include <iostream>
#include <limits.h>
#include <memory> // For std::unique_ptr
#include <sstream>
#include <stdarg.h> // For va_start, etc.
#include <string>
#include <sys/stat.h>

#include "littleEndianAlignment.h"
#include "stringFormatter.h"

namespace KCT {
namespace io {
    void readFirstBytes(std::string fileName, uint8_t* buffer, uint64_t numBytes);
    void
    readBytesFrom(std::string fileName, uint64_t fromPosition, uint8_t* buffer, uint64_t numBytes);
    void
    readBytesFrom(std::shared_ptr<std::ifstream> fileName, uint64_t fromPosition, uint8_t* buffer, uint64_t numBytes);
    void writeFirstBytes(std::string fileName, uint8_t* buffer, uint64_t numBytes);
    void writeBytesFrom(std::shared_ptr<std::ofstream> ofstream,
                        uint64_t fromPosition,
                        uint8_t* buffer,
                        uint64_t numBytes);
    void
    writeBytesFrom(std::string fileName, uint64_t fromPosition, uint8_t* buffer, uint64_t numBytes);
    void appendBytes(std::string fileName, uint8_t* buffer, uint64_t numBytes);
    void createEmptyFile(std::string fileName, uint64_t numBytes, bool overwrite);
    bool pathExists(std::string fileName);
    bool isDirectory(const std::experimental::filesystem::path& p);
    bool isRegularFile(const std::experimental::filesystem::path& p);
    long getFileSize(std::string filename);
    std::string getParent(const std::string& path);
    std::string getBasename(const std::string& path);
    std::string fileToString(const std::string& fileName);
    /**
     * Concatenate text files given by inputFiles into the file given by outputFile.
     * Intended to create compilation of cl files into the file allsources.cl
     * Example:
     * @code
     *    std::string xpath = "/path/to/files"
     *    clFile = io::xprintf("%s/opencl/allsources.cl", xpath.c_str());
     *    io::concatenateTextFiles(clFile, true,
     *                             { io::xprintf("%s/opencl/utils.cl", xpath.c_str()),
     *                               io::xprintf("%s/opencl/projector.cl", xpath.c_str()),
     *                               io::xprintf("%s/opencl/backprojector.cl", xpath.c_str()) });
     *
     * @endcode
     *
     * @param outputFile Output file string.
     * @param overwrite Should overwrite output file.
     * @param inputFiles Input files as a std::initializer_list should be strings put into {}
     * brackets.
     */
    void concatenateTextFiles(const std::string& outputFile,
                              bool overwrite,
                              std::initializer_list<std::string> inputFiles);
    void concatenateTextFiles(const std::string& outputFile,
                              bool overwrite,
                              std::vector<std::string>& inputFiles);
} // namespace io
} // namespace KCT

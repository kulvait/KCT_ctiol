#pragma once
// External libraries
#include <fstream>
#include <iostream>
#include <vector>
// Internal libraries
#include "rawop.h"

namespace KCT {
namespace io {
    /**
     *Interface for reading Frame2D objects.
     *
     *The implementation might be "slim implementation" that access underlying source each time it
     *is called. The implementation might be also "fat implementation" that holds in memory
     *structure that source each call.
     */
    class CSVWriter
    {
    public:
        CSVWriter(std::string fileName, std::string separator, bool overwrite);
        void writeLine(std::string text);
        void writeText(std::string text);
        void writeSeparator();
        void writeNewline();
        void writeVector(std::string name, std::vector<double> v);
        void close();

    private:
        std::string fileName;
        const std::string separator;
        bool overwrite;
        std::ofstream ofs;
    };
} // namespace io
} // namespace KCT

#include "CSVWriter.hpp"

using namespace KCT::io;

CSVWriter::CSVWriter(std::string fileName, std::string separator, bool overwrite)
    : fileName(fileName)
    , separator(separator)
    , overwrite(overwrite)
{
    std::string err;
    if(io::pathExists(fileName))
    {
        if(overwrite)
        {
            ofs.open(fileName);
        } else
        {
            err = io::xprintf("File %s exist and overwrite is not set");
        }
    } else
    {
        ofs.open(fileName);
    }
}

void CSVWriter::writeText(std::string text) { ofs << text; }

void CSVWriter::writeLine(std::string text)
{
    ofs << text;
    ofs << std::endl;
}

void CSVWriter::writeSeparator() { ofs << separator; }
void CSVWriter::writeNewline() { ofs << std::endl; }

void CSVWriter::writeVector(std::string name, std::vector<double> vec)
{
    writeText(name);
    for(double v : vec)
    {
        writeSeparator();
        writeText(io::xprintf("%f", v));
    }
    writeNewline();
}

void CSVWriter::close()
{
    ofs.flush();
    ofs.close();
}

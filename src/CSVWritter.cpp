#include "CSVWritter.hpp"

using namespace CTL::io;

CSVWritter::CSVWritter(std::string fileName, std::string separator, bool overwrite)
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

void CSVWritter::writeText(std::string text) { ofs << text; }

void CSVWritter::writeLine(std::string text)
{
    ofs << text;
    ofs << std::endl;
}

void CSVWritter::writeSeparator() { ofs << separator; }
void CSVWritter::writeNewline() { ofs << std::endl; }

void CSVWritter::writeVector(std::string name, std::vector<double> vec)
{
    writeText(name);
    for(double v : vec)
    {
        writeSeparator();
        writeText(io::xprintf("%f", v));
    }
    writeNewline();
}

void CSVWritter::close()
{
    ofs.flush();
    ofs.close();
}

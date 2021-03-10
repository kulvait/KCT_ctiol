#include "catch.hpp"
// Internal libs
#include "CSVWriter.hpp"

using namespace CTL;

std::string basedir(); // Defined in main file so that it will be accessible to linker

TEST_CASE("TEST: CSVWriter", "[csvwritter][NOPRINT][NOVIZ]")
{
    std::string fileName = "/tmp/test.csv";
    io::CSVWriter csv(fileName, "\t", true);
    csv.writeLine("Ahoj");
    std::vector<double> val;
    for(int i = 0; i != 10; i++)
    {
        val.emplace_back(double(i));
    }
    csv.writeVector("Test vector", val);
    csv.close();
}

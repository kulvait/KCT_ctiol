// Logging, testing
#include "catch.hpp"
#include <plog/Log.h>

// Standard libs
#include <cmath>
#include <iostream>

// Internal libs
#include "CTIOL.h"

using namespace CTL;

TEST_CASE("TEST: concatenateTextFiles function.", "rawop.h")
{
    std::string fileA = "../tests/testFiles/A";
    std::string fileB = "../tests/testFiles/B";
    std::string outputFile = "/tmp/AB";
    io::concatenateTextFiles(outputFile, true, { fileA, fileB });
    std::string content = io::fileToString(outputFile);
    REQUIRE(content == "A\nB\n");
}

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

TEST_CASE("TEST: absolute path.", "rawop.h")
{
    std::string p = io::getParent("abc");
    //LOGE << p;
}

TEST_CASE("Filesystem operations", "[filesystem]")
{
    REQUIRE(io::pathExists("/tmp"));
    REQUIRE(io::isDirectory("/tmp"));
    REQUIRE(!io::isRegularFile("/tmp"));
    //LOGE << p;
}

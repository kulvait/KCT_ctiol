// Logging, testing
#include "catch.hpp"
#include <plog/Log.h>

// Standard libs
#include <cmath>
#include <iostream>

// Internal libs
#include "CTIOL.h"

using namespace CTL;

std::string basedir();//Included in main

TEST_CASE("TEST: concatenateTextFiles function.", "rawop.h")
{
    std::string fileA = io::xprintf("%s/tests/testFiles/A", basedir().c_str());
    std::string fileB = io::xprintf("%s/tests/testFiles/B", basedir().c_str());
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

//Logging, testing
#include "catch2/catch.hpp"
#include <plog/Log.h>

//Standard libs
#include <iostream>
#include <cmath>

//Internal libs
#include "CTIOL.h"

using namespace CTL;

uint8_t uint8Value(int i)
{
    uint8_t buffer[100];
    io::readFirstBytes("../tests/integerByteManipulations/uint8", buffer, 2);
    return util::nextUint8(&buffer[i]);
}

TEST_CASE("TEST: putUint8 function.", "convertEndians_test.cpp")
{
    std::string file = "../tests/integerByteManipulations/uint8_";
    uint8_t buffer[100];
    util::putUint8(0, &buffer[0]);
    util::putUint8(255, &buffer[1]);
    io::writeFirstBytes(file, buffer, 2);
    io::readFirstBytes(file, buffer, 2);
    REQUIRE(util::nextUint8(&buffer[0]) == 0);
    REQUIRE(util::nextUint8(&buffer[1]) == 255);
}

TEST_CASE("TEST: nextUint8 function.", "convertEndians_test.cpp")
{
    REQUIRE(uint8Value(0) == 0);
    REQUIRE(uint8Value(1) == 255);
}

TEST_CASE("TEST: nextUint16 function.", "convertEndians_test.cpp")
{
    uint8_t buffer[100];
    uint8_t xuffer[100];
    io::readFirstBytes("../tests/integerByteManipulations/uint16", buffer, 4);
    REQUIRE(util::nextUint16(&buffer[0]) == 0);
    REQUIRE(util::nextUint16(&buffer[2]) == 65535);
    SECTION("Now testing writing")
    {
        std::string file = "../tests/integerByteManipulations/uint16_";
        util::putUint16(0, &buffer[0]);
        util::putUint16(65535, &buffer[2]);
        util::putUint16(65533, &buffer[4]);
        io::writeFirstBytes(file, buffer, 6);
        io::readFirstBytes(file, xuffer, 6);
        REQUIRE(util::nextUint16(&xuffer[0]) == 0);
        REQUIRE(util::nextUint16(&xuffer[2]) == 65535);
        REQUIRE(util::nextUint16(&xuffer[4]) == 65533);
    }
}

TEST_CASE("TEST: nextUint32 function.", "convertEndians_test.cpp")
{
    uint8_t buffer[100];
    uint8_t xuffer[100];
    io::readFirstBytes("../tests/integerByteManipulations/uint32", buffer, 8);
    REQUIRE(util::nextUint32(&buffer[0]) == 0);
    REQUIRE(util::nextUint32(&buffer[4]) == 4294967295);
    SECTION("Now testing writing")
    {
        std::string file = "../tests/integerByteManipulations/uint32_";
        util::putUint32(0, &buffer[0]);
        util::putUint32(4294967295, &buffer[4]);
        util::putUint32(3294967195, &buffer[8]);
        io::writeFirstBytes(file, buffer, 12);
        io::readFirstBytes(file, xuffer, 12);
        REQUIRE(util::nextUint32(&xuffer[0]) == 0);
        REQUIRE(util::nextUint32(&xuffer[4]) == 4294967295);
        REQUIRE(util::nextUint32(&xuffer[8]) == 3294967195);
    }
}

TEST_CASE("TEST: nextUint64 function.", "convertEndians_test.cpp")
{
    uint8_t buffer[100];
    uint8_t xuffer[100];
    io::readFirstBytes("../tests/integerByteManipulations/uint64", buffer, 16);
    REQUIRE(util::nextUint64(&buffer[0]) == 0);
    uint64_t maxval = 18446744073709551615ULL;
    REQUIRE(util::nextUint64(&buffer[8]) == maxval);
    SECTION("Now testing writing")
    {
        std::string file = "../tests/integerByteManipulations/uint64_";
        uint64_t valuesToWrite[9] = { 0,
            18446744073709551615ULL,
            3294967195,
            1,
            50000000000ULL,
            2,
            3,
            256,
            4294967295 };
        for (int i = 0; i != 9; i++) {
            util::putUint64(valuesToWrite[i], &buffer[i * 8]);
        }
        io::writeFirstBytes(file, buffer, 72);
        io::readFirstBytes(file, xuffer, 72);
        for (int i = 0; i != 9; i++) {
            REQUIRE(util::nextUint64(&xuffer[i * 8]) == valuesToWrite[i]);
        }
    }
}

TEST_CASE("TEST: nextInt8 readBytesFrom.", "convertEndians_test.cpp")
{
    uint8_t buffer[100];
    uint8_t xuffer[100];
    io::readFirstBytes("../tests/integerByteManipulations/int8", buffer, 5);
    REQUIRE(util::nextInt8(buffer) == -128);
    REQUIRE(util::nextInt8(&buffer[1]) == -1);
    REQUIRE(util::nextInt8(&buffer[2]) == 0);
    REQUIRE(util::nextInt8(&buffer[3]) == 1);
    REQUIRE(util::nextInt8(&buffer[4]) == 127);
    SECTION("Now testing writing")
    {
        int TYPESIZE = 1;
        std::string file = "../tests/integerByteManipulations/int8_";
        int8_t valuesToWrite[9] = { -128,
            -1,
            0,
            1,
            127, -2, -3, 33, 21 };
        for (int i = 0; i != 9; i++) {
            util::putInt8(valuesToWrite[i], &buffer[i * TYPESIZE]);
        }
        io::writeFirstBytes(file, buffer, 9 * TYPESIZE);
        io::readFirstBytes(file, xuffer, 9 * TYPESIZE);
        for (int i = 0; i != 9; i++) {
            REQUIRE(util::nextInt8(&xuffer[i * TYPESIZE]) == valuesToWrite[i]);
        }
    }
}

TEST_CASE("TEST: nextInt16 readBytesFrom.", "convertEndians_test.cpp")
{
    uint8_t buffer[100];
    uint8_t xuffer[100];
    io::readFirstBytes("../tests/integerByteManipulations/int16", buffer, 10);
    REQUIRE(util::nextInt16(buffer) == -32768);
    REQUIRE(util::nextInt16(&buffer[2]) == -1);
    REQUIRE(util::nextInt16(&buffer[4]) == 0);
    REQUIRE(util::nextInt16(&buffer[6]) == 1);
    REQUIRE(util::nextInt16(&buffer[8]) == 32767);
    SECTION("Now testing writing")
    {
        int TYPESIZE = 2;
        std::string file = "../tests/integerByteManipulations/int16_";
        int16_t valuesToWrite[9] = { -128,
            -1,
            0,
            1,
            127, -32768, 32767, -3, 999 };
        for (int i = 0; i != 9; i++) {
            util::putInt16(valuesToWrite[i], &buffer[i * TYPESIZE]);
        }
        io::writeFirstBytes(file, buffer, 9 * TYPESIZE);
        io::readFirstBytes(file, xuffer, 9 * TYPESIZE);
        for (int i = 0; i != 9; i++) {
            REQUIRE(util::nextInt16(&xuffer[i * TYPESIZE]) == valuesToWrite[i]);
        }
    }
}

TEST_CASE("TEST: nextInt32 readBytesFrom.", "convertEndians_test.cpp")
{
    uint8_t buffer[100];
    uint8_t xuffer[100];
    io::readFirstBytes("../tests/integerByteManipulations/int32", buffer, 20);
    REQUIRE(util::nextInt32(buffer) == -2147483648);
    REQUIRE(util::nextInt32(&buffer[4]) == -1);
    REQUIRE(util::nextInt32(&buffer[8]) == 0);
    REQUIRE(util::nextInt32(&buffer[12]) == 1);
    REQUIRE(util::nextInt32(&buffer[16]) == 2147483647);
    SECTION("Now testing writing")
    {
        int TYPESIZE = 4;
        std::string file = "../tests/integerByteManipulations/int32_";
        int32_t valuesToWrite[9] = { -128,
            -1,
            0,
            1,
            127, -32768, 32767, -2147483648, 2147483647 };
        for (int i = 0; i != 9; i++) {
            util::putInt32(valuesToWrite[i], &buffer[i * TYPESIZE]);
        }
        io::writeFirstBytes(file, buffer, 9 * TYPESIZE);
        io::readFirstBytes(file, xuffer, 9 * TYPESIZE);
        for (int i = 0; i != 9; i++) {
            REQUIRE(util::nextInt32(&xuffer[i * TYPESIZE]) == valuesToWrite[i]);
        }
    }
}

TEST_CASE("TEST: nextInt64, putInt64, readBytesFrom.", "convertEndians_test.cpp")
{
    uint8_t buffer[100];
    uint8_t xuffer[100];
    int TYPESIZE = 8;
    std::string file = "../tests/integerByteManipulations/int32_";
    int64_t valuesToWrite[11] = { -128,
        -1,
        0,
        1,
        127, -32768, 32767, -2147483648, 2147483647, -9223372036854775807LL, 9223372036854775807LL }; //Signed range of int64_t is restricted to −9,223,372,036,854,775,807 to +9,223,372,036,854,775,807 not 9,223,372,036,854,775,808 to +9,223,372,036,854,775,807
    for (int i = 0; i != 11; i++) {
        util::putInt64(valuesToWrite[i], &buffer[i * TYPESIZE]);
    }
    io::writeFirstBytes(file, buffer, 11 * TYPESIZE);
    io::readFirstBytes(file, xuffer, 11 * TYPESIZE);
    for (int i = 0; i != 11; i++) {
        REQUIRE(util::nextInt64(&xuffer[i * TYPESIZE]) == valuesToWrite[i]);
    }
}

TEST_CASE("TEST: nextFloat readBytesFrom.", "convertEndians_test.cpp")
{
    uint8_t buffer[100];
    uint8_t xuffer[100];
    io::readFirstBytes("../tests/integerByteManipulations/float", buffer, 20);
    REQUIRE(util::nextFloat(buffer) == 1.1f);
    REQUIRE(util::nextFloat(&buffer[4]) == 3.1f);
    REQUIRE(util::nextFloat(&buffer[8]) == 340282300000000000000000000000000000000.0f);
    REQUIRE(util::nextFloat(&buffer[12]) == -3402.823f);
    REQUIRE(util::nextFloat(&buffer[16]) == -3.0e25f);
    SECTION("Now test writing.")
    {
        int TYPESIZE = 4;
        std::string file = "../tests/integerByteManipulations/float_";
        float valuesToWrite[11] = { 1.1f,
            3.1f,
            340282300000000000000000000000000000000.0f,
            -3402.823f,
            -3.0e25f, 1.0f, 12.32f, -8.1f, std::numeric_limits<float>::quiet_NaN(), std::numeric_limits<float>::infinity(), -std::numeric_limits<float>::infinity() };
        for (int i = 0; i != 11; i++) {
            util::putFloat(valuesToWrite[i], &buffer[i * TYPESIZE]);
        }
        io::writeFirstBytes(file, buffer, 11 * TYPESIZE);
        io::readFirstBytes(file, xuffer, 11 * TYPESIZE);
        for (int i = 0; i != 11; i++) {
            float x = util::nextFloat(&xuffer[i * TYPESIZE]);
            if (std::isnan(x)) {
                REQUIRE(std::isnan(valuesToWrite[i]));
            } else {
                REQUIRE(util::nextFloat(&xuffer[i * TYPESIZE]) == valuesToWrite[i]);
            }
        }
    }
}

TEST_CASE("TEST: nextDouble readBytesFrom.", "convertEndians_test.cpp")
{
    uint8_t buffer[100];
    uint8_t xuffer[100];
    io::readFirstBytes("../tests/integerByteManipulations/double", buffer, 40);
    REQUIRE(util::nextDouble(buffer) == 1.1);
    REQUIRE(util::nextDouble(&buffer[8]) == 3.1);
    REQUIRE(util::nextDouble(&buffer[16]) == 500.0);
    REQUIRE(util::nextDouble(&buffer[24]) == 3.0e253);
    REQUIRE(util::nextDouble(&buffer[32]) == -3.0e253);
    SECTION("Now test  writing")
    {

        int TYPESIZE = 8;
        std::string file = "../tests/integerByteManipulations/double_";
        double valuesToWrite[11] = { 1.1,
            3.1,
            500.0,
            3.0e253,
            -3.0e253,
            1e20,
            32767e1,
            -2147483648.0,
            2147483647.1,
            std::numeric_limits<float>::infinity(),
            -std::numeric_limits<float>::infinity() };
        for (int i = 0; i != 11; i++) {
            util::putDouble(valuesToWrite[i], &buffer[i * TYPESIZE]);
        }
        io::writeFirstBytes(file, buffer, 11 * TYPESIZE);
        io::readFirstBytes(file, xuffer, 11 * TYPESIZE);
        for (int i = 0; i != 11; i++) {
            double x = util::nextDouble(&xuffer[i * TYPESIZE]);
            if (std::isnan(x)) {
                REQUIRE(std::isnan(valuesToWrite[i]));
            } else {
                REQUIRE(util::nextDouble(&xuffer[i * TYPESIZE]) == valuesToWrite[i]);
            }
        }
    }
}

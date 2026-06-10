// Logging, testing
#include "catch.hpp"
#include <plog/Log.h>

// Standard libs
#include <cmath>
#include <cstring>
#include <fstream>
#include <iostream>
#include <memory>

// Internal libs
#include "CTIOL.h"

using namespace KCT;

std::string basedir(); // Included in main

// ============================================================================
// pathExists
// ============================================================================
TEST_CASE("pathExists: existing directory", "[rawop][pathExists]")
{
    REQUIRE(io::pathExists("/tmp"));
}

TEST_CASE("pathExists: existing file", "[rawop][pathExists]")
{
    std::string fileA = io::xprintf("%s/tests/testFiles/A", basedir().c_str());
    REQUIRE(io::pathExists(fileA));
}

TEST_CASE("pathExists: non-existing path", "[rawop][pathExists]")
{
    REQUIRE_FALSE(io::pathExists("/tmp/this_path_should_not_exist_rawop_test_xyz"));
}

// ============================================================================
// isDirectory / isRegularFile
// ============================================================================
TEST_CASE("isDirectory: on directory", "[rawop][isDirectory]")
{
    REQUIRE(io::isDirectory("/tmp"));
}

TEST_CASE("isDirectory: on regular file returns false", "[rawop][isDirectory]")
{
    std::string fileA = io::xprintf("%s/tests/testFiles/A", basedir().c_str());
    REQUIRE_FALSE(io::isDirectory(fileA));
}

TEST_CASE("isRegularFile: on regular file", "[rawop][isRegularFile]")
{
    std::string fileA = io::xprintf("%s/tests/testFiles/A", basedir().c_str());
    REQUIRE(io::isRegularFile(fileA));
}

TEST_CASE("isRegularFile: on directory returns false", "[rawop][isRegularFile]")
{
    REQUIRE_FALSE(io::isRegularFile("/tmp"));
}

// ============================================================================
// getFileSize
// ============================================================================
TEST_CASE("getFileSize: known file", "[rawop][getFileSize]")
{
    // File A contains "A\n" = 2 bytes
    std::string fileA = io::xprintf("%s/tests/testFiles/A", basedir().c_str());
    REQUIRE(io::getFileSize(fileA) == 2);
}

TEST_CASE("getFileSize: non-existing file returns -1", "[rawop][getFileSize]")
{
    REQUIRE(io::getFileSize("/tmp/nonexistent_rawop_test_xyz") == -1);
}

// ============================================================================
// getParent / getBasename
// ============================================================================
TEST_CASE("getBasename: path with directory", "[rawop][getBasename]")
{
    REQUIRE(io::getBasename("/some/path/to/file.txt") == "file.txt");
}

TEST_CASE("getBasename: bare filename", "[rawop][getBasename]")
{
    REQUIRE(io::getBasename("file.txt") == "file.txt");
}

TEST_CASE("getBasename: trailing slash", "[rawop][getBasename]")
{
    // After last '/', empty string
    REQUIRE(io::getBasename("/some/path/") == "");
}

TEST_CASE("getParent: relative path", "[rawop][getParent]")
{
    // Should return some absolute path without throwing
    std::string p = io::getParent("abc");
    REQUIRE_FALSE(p.empty());
}

// ============================================================================
// fileToString
// ============================================================================
TEST_CASE("fileToString: reads file A correctly", "[rawop][fileToString]")
{
    std::string fileA = io::xprintf("%s/tests/testFiles/A", basedir().c_str());
    std::string content = io::fileToString(fileA);
    REQUIRE(content == "A\n");
}

TEST_CASE("fileToString: reads file B correctly", "[rawop][fileToString]")
{
    std::string fileB = io::xprintf("%s/tests/testFiles/B", basedir().c_str());
    std::string content = io::fileToString(fileB);
    REQUIRE(content == "B\n");
}

TEST_CASE("fileToString: non-existing file throws", "[rawop][fileToString]")
{
    REQUIRE_THROWS(io::fileToString("/tmp/nonexistent_rawop_test_xyz"));
}

TEST_CASE("fileToString: directory throws", "[rawop][fileToString]")
{
    REQUIRE_THROWS(io::fileToString("/tmp"));
}

// ============================================================================
// createEmptyFile
// ============================================================================
TEST_CASE("createEmptyFile: creates file with correct size", "[rawop][createEmptyFile]")
{
    std::string path = "/tmp/rawop_test_empty_file";
    io::createEmptyFile(path, 64, true);
    REQUIRE(io::pathExists(path));
    REQUIRE(io::getFileSize(path) == 64);
    std::remove(path.c_str());
}

TEST_CASE("createEmptyFile: zero-byte file", "[rawop][createEmptyFile]")
{
    std::string path = "/tmp/rawop_test_empty_zero";
    io::createEmptyFile(path, 0, true);
    REQUIRE(io::pathExists(path));
    REQUIRE(io::getFileSize(path) == 0);
    std::remove(path.c_str());
}

TEST_CASE("createEmptyFile: overwrite=false throws on existing", "[rawop][createEmptyFile]")
{
    std::string path = "/tmp/rawop_test_no_overwrite";
    io::createEmptyFile(path, 10, true);
    REQUIRE_THROWS(io::createEmptyFile(path, 10, false));
    std::remove(path.c_str());
}

TEST_CASE("createEmptyFile: overwrite=true replaces existing", "[rawop][createEmptyFile]")
{
    std::string path = "/tmp/rawop_test_overwrite";
    io::createEmptyFile(path, 100, true);
    io::createEmptyFile(path, 32, true);
    REQUIRE(io::getFileSize(path) == 32);
    std::remove(path.c_str());
}

// ============================================================================
// readFirstBytes / writeFirstBytes
// ============================================================================
TEST_CASE("readFirstBytes: reads correct data", "[rawop][readFirstBytes]")
{
    std::string fileA = io::xprintf("%s/tests/testFiles/A", basedir().c_str());
    uint8_t buf[2];
    io::readFirstBytes(fileA, buf, 2);
    REQUIRE(buf[0] == 'A');
    REQUIRE(buf[1] == '\n');
}

TEST_CASE("readFirstBytes: requesting too many bytes throws", "[rawop][readFirstBytes]")
{
    std::string fileA = io::xprintf("%s/tests/testFiles/A", basedir().c_str());
    uint8_t buf[1024];
    REQUIRE_THROWS(io::readFirstBytes(fileA, buf, 1024));
}

TEST_CASE("writeFirstBytes and readFirstBytes roundtrip", "[rawop][writeFirstBytes]")
{
    std::string path = "/tmp/rawop_test_write_first";
    io::createEmptyFile(path, 8, true);
    uint8_t writeData[4] = { 0xDE, 0xAD, 0xBE, 0xEF };
    io::writeFirstBytes(path, writeData, 4);
    uint8_t readData[4];
    io::readFirstBytes(path, readData, 4);
    REQUIRE(std::memcmp(writeData, readData, 4) == 0);
    std::remove(path.c_str());
}

// ============================================================================
// readBytesFrom / writeBytesFrom (string overloads)
// ============================================================================
TEST_CASE("readBytesFrom: reads from offset", "[rawop][readBytesFrom]")
{
    std::string fileA = io::xprintf("%s/tests/testFiles/A", basedir().c_str());
    uint8_t buf[1];
    io::readBytesFrom(fileA, 1, buf, 1);
    REQUIRE(buf[0] == '\n');
}

TEST_CASE("writeBytesFrom: writes at offset", "[rawop][writeBytesFrom]")
{
    std::string path = "/tmp/rawop_test_write_from";
    io::createEmptyFile(path, 8, true);
    uint8_t data[2] = { 0x42, 0x43 };
    io::writeBytesFrom(path, 4, data, 2);
    uint8_t readBuf[2];
    io::readBytesFrom(path, 4, readBuf, 2);
    REQUIRE(readBuf[0] == 0x42);
    REQUIRE(readBuf[1] == 0x43);
    std::remove(path.c_str());
}

// ============================================================================
// readBytesFrom / writeBytesFrom (stream overloads)
// ============================================================================
TEST_CASE("readBytesFrom: ifstream overload", "[rawop][readBytesFrom][stream]")
{
    std::string fileA = io::xprintf("%s/tests/testFiles/A", basedir().c_str());
    auto ifs = std::make_shared<std::ifstream>(fileA, std::ios::binary | std::ios::in);
    REQUIRE(ifs->is_open());
    uint8_t buf[1];
    io::readBytesFrom(ifs, 0, buf, 1);
    REQUIRE(buf[0] == 'A');
    io::readBytesFrom(ifs, 1, buf, 1);
    REQUIRE(buf[0] == '\n');
    ifs->close();
}

TEST_CASE("writeBytesFrom: ofstream overload", "[rawop][writeBytesFrom][stream]")
{
    std::string path = "/tmp/rawop_test_write_stream";
    io::createEmptyFile(path, 16, true);
    auto ofs = std::make_shared<std::ofstream>(path, std::ios::binary | std::ios::in | std::ios::out);
    REQUIRE(ofs->is_open());
    uint8_t data[3] = { 'X', 'Y', 'Z' };
    io::writeBytesFrom(ofs, 5, data, 3);
    ofs->close();
    uint8_t readBuf[3];
    io::readBytesFrom(path, 5, readBuf, 3);
    REQUIRE(readBuf[0] == 'X');
    REQUIRE(readBuf[1] == 'Y');
    REQUIRE(readBuf[2] == 'Z');
    std::remove(path.c_str());
}

// ============================================================================
// appendBytes
// ============================================================================
TEST_CASE("appendBytes: appends to existing file", "[rawop][appendBytes]")
{
    std::string path = "/tmp/rawop_test_append";
    io::createEmptyFile(path, 4, true);
    REQUIRE(io::getFileSize(path) == 4);
    uint8_t data[3] = { 'A', 'B', 'C' };
    io::appendBytes(path, data, 3);
    REQUIRE(io::getFileSize(path) == 7);
    uint8_t readBuf[3];
    io::readBytesFrom(path, 4, readBuf, 3);
    REQUIRE(readBuf[0] == 'A');
    REQUIRE(readBuf[1] == 'B');
    REQUIRE(readBuf[2] == 'C');
    std::remove(path.c_str());
}

// ============================================================================
// concatenateTextFiles
// ============================================================================
TEST_CASE("concatenateTextFiles: initializer_list overload", "[rawop][concatenateTextFiles]")
{
    std::string fileA = io::xprintf("%s/tests/testFiles/A", basedir().c_str());
    std::string fileB = io::xprintf("%s/tests/testFiles/B", basedir().c_str());
    std::string outputFile = "/tmp/rawop_test_concat_init";
    io::concatenateTextFiles(outputFile, true, { fileA, fileB });
    std::string content = io::fileToString(outputFile);
    REQUIRE(content == "A\nB\n");
    std::remove(outputFile.c_str());
}

TEST_CASE("concatenateTextFiles: vector overload", "[rawop][concatenateTextFiles]")
{
    std::string fileA = io::xprintf("%s/tests/testFiles/A", basedir().c_str());
    std::string fileB = io::xprintf("%s/tests/testFiles/B", basedir().c_str());
    std::vector<std::string> files = { fileA, fileB };
    std::string outputFile = "/tmp/rawop_test_concat_vec";
    io::concatenateTextFiles(outputFile, true, files);
    std::string content = io::fileToString(outputFile);
    REQUIRE(content == "A\nB\n");
    std::remove(outputFile.c_str());
}

TEST_CASE("concatenateTextFiles: single file", "[rawop][concatenateTextFiles]")
{
    std::string fileA = io::xprintf("%s/tests/testFiles/A", basedir().c_str());
    std::string outputFile = "/tmp/rawop_test_concat_single";
    io::concatenateTextFiles(outputFile, true, { fileA });
    std::string content = io::fileToString(outputFile);
    REQUIRE(content == "A\n");
    std::remove(outputFile.c_str());
}

TEST_CASE("concatenateTextFiles: overwrite=false throws on existing output", "[rawop][concatenateTextFiles]")
{
    std::string fileA = io::xprintf("%s/tests/testFiles/A", basedir().c_str());
    std::string outputFile = "/tmp/rawop_test_concat_nooverwrite";
    io::concatenateTextFiles(outputFile, true, { fileA });
    REQUIRE_THROWS(io::concatenateTextFiles(outputFile, false, { fileA }));
    std::remove(outputFile.c_str());
}

TEST_CASE("concatenateTextFiles: non-existing input file throws", "[rawop][concatenateTextFiles]")
{
    std::string outputFile = "/tmp/rawop_test_concat_badfile";
    REQUIRE_THROWS(io::concatenateTextFiles(outputFile, true, { "/tmp/nonexistent_xyz" }));
}

// ============================================================================
// filesToString
// ============================================================================
TEST_CASE("filesToString: vector overload concatenates files in memory", "[rawop][filesToString]")
{
    std::string fileA = io::xprintf("%s/tests/testFiles/A", basedir().c_str());
    std::string fileB = io::xprintf("%s/tests/testFiles/B", basedir().c_str());
    std::vector<std::string> files = { fileA, fileB };
    std::string content = io::filesToString(files);
    REQUIRE(content == "A\nB\n");
}

TEST_CASE("filesToString: initializer_list overload", "[rawop][filesToString]")
{
    std::string fileA = io::xprintf("%s/tests/testFiles/A", basedir().c_str());
    std::string fileB = io::xprintf("%s/tests/testFiles/B", basedir().c_str());
    std::string content = io::filesToString({ fileA, fileB });
    REQUIRE(content == "A\nB\n");
}

TEST_CASE("filesToString: single file", "[rawop][filesToString]")
{
    std::string fileA = io::xprintf("%s/tests/testFiles/A", basedir().c_str());
    std::vector<std::string> files = { fileA };
    std::string content = io::filesToString(files);
    REQUIRE(content == "A\n");
}

TEST_CASE("filesToString: empty vector returns empty string", "[rawop][filesToString]")
{
    std::vector<std::string> files;
    std::string content = io::filesToString(files);
    REQUIRE(content.empty());
}

TEST_CASE("filesToString: non-existing file throws", "[rawop][filesToString]")
{
    std::vector<std::string> files = { "/tmp/nonexistent_rawop_test_xyz" };
    REQUIRE_THROWS(io::filesToString(files));
}

TEST_CASE("filesToString: directory in list throws", "[rawop][filesToString]")
{
    std::vector<std::string> files = { "/tmp" };
    REQUIRE_THROWS(io::filesToString(files));
}

TEST_CASE("filesToString: matches concatenateTextFiles output", "[rawop][filesToString]")
{
    std::string fileA = io::xprintf("%s/tests/testFiles/A", basedir().c_str());
    std::string fileB = io::xprintf("%s/tests/testFiles/B", basedir().c_str());
    std::vector<std::string> files = { fileA, fileB };

    // filesToString result
    std::string memResult = io::filesToString(files);

    // concatenateTextFiles result
    std::string outputFile = "/tmp/rawop_test_compare";
    io::concatenateTextFiles(outputFile, true, files);
    std::string fileResult = io::fileToString(outputFile);

    REQUIRE(memResult == fileResult);
    std::remove(outputFile.c_str());
}

TEST_CASE("filesToString: order matters", "[rawop][filesToString]")
{
    std::string fileA = io::xprintf("%s/tests/testFiles/A", basedir().c_str());
    std::string fileB = io::xprintf("%s/tests/testFiles/B", basedir().c_str());
    std::vector<std::string> ab = { fileA, fileB };
    std::vector<std::string> ba = { fileB, fileA };
    REQUIRE(io::filesToString(ab) == "A\nB\n");
    REQUIRE(io::filesToString(ba) == "B\nA\n");
}

TEST_CASE("filesToString: duplicate files", "[rawop][filesToString]")
{
    std::string fileA = io::xprintf("%s/tests/testFiles/A", basedir().c_str());
    std::vector<std::string> files = { fileA, fileA, fileA };
    std::string content = io::filesToString(files);
    REQUIRE(content == "A\nA\nA\n");
}
// ============================================================================
// stringToFile
// ============================================================================
TEST_CASE("stringToFile: writes string to new file", "[rawop][stringToFile]")
{
    std::string path = "/tmp/rawop_test_str2file";
    std::string content = "Hello, KCT!\n";
    io::stringToFile(path, true, content);
    REQUIRE(io::pathExists(path));
    REQUIRE(io::fileToString(path) == content);
    std::remove(path.c_str());
}

TEST_CASE("stringToFile: empty string creates empty file", "[rawop][stringToFile]")
{
    std::string path = "/tmp/rawop_test_str2file_empty";
    io::stringToFile(path, true, "");
    REQUIRE(io::pathExists(path));
    REQUIRE(io::getFileSize(path) == 0);
    REQUIRE(io::fileToString(path).empty());
    std::remove(path.c_str());
}

TEST_CASE("stringToFile: overwrite=true replaces existing content", "[rawop][stringToFile]")
{
    std::string path = "/tmp/rawop_test_str2file_overwrite";
    io::stringToFile(path, true, "first content\n");
    io::stringToFile(path, true, "second\n");
    REQUIRE(io::fileToString(path) == "second\n");
    std::remove(path.c_str());
}

TEST_CASE("stringToFile: overwrite=true with shorter content truncates", "[rawop][stringToFile]")
{
    std::string path = "/tmp/rawop_test_str2file_truncate";
    io::stringToFile(path, true, "long content here\n");
    io::stringToFile(path, true, "short\n");
    REQUIRE(io::fileToString(path) == "short\n");
    REQUIRE(io::getFileSize(path) == 6);
    std::remove(path.c_str());
}

TEST_CASE("stringToFile: overwrite=false throws on existing file", "[rawop][stringToFile]")
{
    std::string path = "/tmp/rawop_test_str2file_nooverwrite";
    io::stringToFile(path, true, "existing\n");
    REQUIRE_THROWS(io::stringToFile(path, false, "new\n"));
    // Original content preserved
    REQUIRE(io::fileToString(path) == "existing\n");
    std::remove(path.c_str());
}

TEST_CASE("stringToFile: overwrite=false works on non-existing file", "[rawop][stringToFile]")
{
    std::string path = "/tmp/rawop_test_str2file_nooverwrite_new";
    std::remove(path.c_str()); // ensure it doesn't exist
    io::stringToFile(path, false, "brand new\n");
    REQUIRE(io::fileToString(path) == "brand new\n");
    std::remove(path.c_str());
}

TEST_CASE("stringToFile: binary content roundtrip", "[rawop][stringToFile]")
{
    std::string path = "/tmp/rawop_test_str2file_binary";
    std::string content = { '\x00', '\x01', '\x02', '\xFF', '\xFE', '\xFD' };
    io::stringToFile(path, true, content);
    std::string readBack = io::fileToString(path);
    REQUIRE(readBack.size() == 6);
    REQUIRE(readBack == content);
    std::remove(path.c_str());
}

TEST_CASE("stringToFile: roundtrip with filesToString", "[rawop][stringToFile]")
{
    std::string fileA = io::xprintf("%s/tests/testFiles/A", basedir().c_str());
    std::string fileB = io::xprintf("%s/tests/testFiles/B", basedir().c_str());
    std::vector<std::string> files = { fileA, fileB };

    std::string concatenated = io::filesToString(files);
    std::string path = "/tmp/rawop_test_str2file_roundtrip";
    io::stringToFile(path, true, concatenated);
    REQUIRE(io::fileToString(path) == "A\nB\n");
    std::remove(path.c_str());
}

TEST_CASE("stringToFile: large content", "[rawop][stringToFile]")
{
    std::string path = "/tmp/rawop_test_str2file_large";
    std::string content(1024 * 1024, 'X'); // 1 MB of 'X'
    io::stringToFile(path, true, content);
    REQUIRE(io::getFileSize(path) == 1024 * 1024);
    REQUIRE(io::fileToString(path) == content);
    std::remove(path.c_str());
}

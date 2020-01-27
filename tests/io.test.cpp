#include "catch.hpp"

//Internal libs
#include "rawop.h"
#include "stringFormatter.h"
#include "DEN/DenFileInfo.hpp"
#include "DEN/DenFrame2DReader.hpp"
#include "PROG/RunTimeInfo.hpp"
#include "littleEndianAlignment.h"

using namespace CTL;

std::string basedir();//Defined in main file so that it will be accessible to linker

TEST_CASE("TEST: CTL::io::DenFileInfo", "[denfileinfo][NOPRINT][NOVIZ]")
{
	std::string fileName = io::xprintf("%s/tests/testFiles/CAMERA.matrices", basedir().c_str());
    io::DenFileInfo di(fileName);
    REQUIRE(di.getNumRows() == 3);
    REQUIRE(di.getNumCols() == 4);
    REQUIRE(di.getNumSlices() == 248);
    REQUIRE(di.dimx() == 4);
    REQUIRE(di.dimy() == 3);
    REQUIRE(di.dimz() == 248);
    REQUIRE(di.getSize() == 23808 + 6);
    REQUIRE(di.getNumPixels() == 2976);
    REQUIRE(di.getDataType() == io::DenSupportedType::double_);
    REQUIRE(di.elementByteSize() == 8);
}

TEST_CASE("TEST: CTL::io::DenFrame2DReader", "[denframereader][NOPRINT][NOVIZ]")
{
	std::string fileName = io::xprintf("%s/tests/testFiles/CAMERA.matrices", basedir().c_str());
	io::DenFrame2DReader<double> dfr(fileName);	
    REQUIRE(dfr.dimx() == 4);
    REQUIRE(dfr.dimy() == 3);
    REQUIRE(dfr.dimz() == 248);
}

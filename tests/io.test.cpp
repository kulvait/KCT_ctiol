#include "catch.hpp"

// Internal libs
#include "DEN/DenFileInfo.hpp"
#include "DEN/DenFrame2DCachedReader.hpp"
#include "DEN/DenFrame2DReader.hpp"
#include "PROG/KCTException.hpp"
#include "PROG/RunTimeInfo.hpp"
#include "helpers.test.hpp"
#include "littleEndianAlignment.h"
#include "rawop.h"
#include "stringFormatter.h"

using namespace KCT;

std::string basedir(); // Defined in main file so that it will be accessible to linker

TEST_CASE("TEST: KCT::io::DenFileInfo", "[denfileinfo][NOPRINT][NOVIZ]")
{
    std::string fileName = io::xprintf("%s/tests/testFiles/CAMERA.matrices", basedir().c_str());
    io::DenFileInfo di(fileName);
    REQUIRE(di.getNumRows() == 3);
    REQUIRE(di.getNumCols() == 4);
    REQUIRE(di.getNumSlices() == 248);
    REQUIRE(di.dimx() == 4);
    REQUIRE(di.dimy() == 3);
    REQUIRE(di.dimz() == 248);
    REQUIRE(di.getFrameCount() == 248);
    REQUIRE(di.getFrameSize() == 12);
    REQUIRE(di.getFrameByteSize() == 96);
    REQUIRE(di.getOffset() == 6);
    REQUIRE(!di.isExtended());
    REQUIRE(di.hasXMajorAlignment());
    REQUIRE(di.getFileSize() == 23808 + 6);
    REQUIRE(di.getElementCount() == 2976);
    REQUIRE(di.getElementType() == io::DenSupportedType::FLOAT64);
    REQUIRE(di.getElementByteSize() == 8);
}

TEST_CASE("TEST: KCT::io::DenFrame2DReader", "[denframereader][NOPRINT][NOVIZ]")
{
    std::string fileName = io::xprintf("%s/tests/testFiles/CAMERA.matrices", basedir().c_str());
    bool kctexception = false;
    try
    {
        // It is not float but double
        io::DenFrame2DReader<float> dfr(fileName, 5);
        std::shared_ptr<io::BufferedFrame2D<float>> f0 = dfr.readBufferedFrame(0);
        std::shared_ptr<io::BufferedFrame2D<float>> f1 = dfr.readBufferedFrame(1);
        std::shared_ptr<io::BufferedFrame2D<float>> f2 = dfr.readBufferedFrame(2);
        REQUIRE(f1->get(0, 0) == static_cast<float>(-0.051792586667711887));
        REQUIRE(f1->get(1, 0) == static_cast<float>(2.6190912906740373));
        REQUIRE(f1->get(2, 0) == static_cast<float>(-0.0015616874131109456));
        REQUIRE(f1->get(3, 0) == static_cast<float>(310.38377533935176));
        REQUIRE(f1->get(0, 1) == static_cast<float>(-0.19344971859001514));
        REQUIRE(f1->get(1, 1) == static_cast<float>(0.022454503444133914));
        REQUIRE(f1->get(2, 1) == static_cast<float>(-2.5920980424069033));
        REQUIRE(f1->get(3, 1) == static_cast<float>(139.55550220066908));
        REQUIRE(f1->get(0, 2) == static_cast<float>(-0.0013223560620414226));
        REQUIRE(f1->get(1, 2) == static_cast<float>(0.00016604466369883966));
        REQUIRE(f1->get(2, 2) == static_cast<float>(9.1904376232148155e-07));
        REQUIRE(f1->get(3, 2) == static_cast<float>(1.0));
    } catch(KCT::util::KCTException e)
    {
        kctexception = true;
    }
    REQUIRE(!kctexception);
    io::DenFrame2DReader<double> dfr(fileName, 5);
    REQUIRE(dfr.dimx() == 4);
    REQUIRE(dfr.dimy() == 3);
    REQUIRE(dfr.getFrameCount() == 248);
    std::shared_ptr<io::BufferedFrame2D<double>> f0 = dfr.readBufferedFrame(0);
    std::shared_ptr<io::BufferedFrame2D<double>> f1 = dfr.readBufferedFrame(1);
    std::shared_ptr<io::BufferedFrame2D<double>> f2 = dfr.readBufferedFrame(2);
    REQUIRE(f1->get(0, 0) == -0.051792586667711887);
    REQUIRE(f1->get(1, 0) == 2.6190912906740373);
    REQUIRE(f1->get(2, 0) == -0.0015616874131109456);
    REQUIRE(f1->get(3, 0) == 310.38377533935176);
    REQUIRE(f1->get(0, 1) == -0.19344971859001514);
    REQUIRE(f1->get(1, 1) == 0.022454503444133914);
    REQUIRE(f1->get(2, 1) == -2.5920980424069033);
    REQUIRE(f1->get(3, 1) == 139.55550220066908);
    REQUIRE(f1->get(0, 2) == -0.0013223560620414226);
    REQUIRE(f1->get(1, 2) == 0.00016604466369883966);
    REQUIRE(f1->get(2, 2) == 9.1904376232148155e-07);
    REQUIRE(f1->get(3, 2) == 1.0);
    io::DenFrame2DCachedReader<double> dfcr(fileName, 5, 5);
    f2 = dfcr.readBufferedFrame(1);
    for(uint32_t i = 0; i != 4; i++)
    {
        for(uint32_t j = 0; j != 3; j++)
        {
            REQUIRE(f1->get(i, j) == f2->get(i, j));
        }
    }
}

TEST_CASE("TEST: KCT::io::KCTException", "[NOPRINT][NOVIZ]")
{
    std::string msg = "Test exception handling";
    try
    {
        KCTERR(msg);
    } catch(util::KCTException& e)
    {
        std::string str = e.get_msg();
        REQUIRE(str.find(msg) != std::string::npos);
    }
}

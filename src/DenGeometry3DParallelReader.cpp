#include "DEN/DenGeometry3DParallelReader.hpp"

namespace KCT::io {

void DenGeometry3DParallelReader::initObject()
{
    std::string ERR;
    geometriesCount = matrixReader->getFrameCount();
    DenSupportedType dt = matrixReader->getDataType();
    if(matrixReader->dimx() != 4 || matrixReader->dimy() != 2)
    {
        ERR = io::xprintf(
            "Dimensions of the file %s shall be dimx=4 and dimy=2 but they are dimx=%d dimy=%d.",
            projectionMatrixFile.c_str(), matrixReader->dimx(), matrixReader->dimy());
        KCTERR(ERR);
    }

    if(dt != io::DenSupportedType::FLOAT64)
    {
        ERR = io::xprintf("Element type of the file %s shall be double but it is %s!",
                          projectionMatrixFile.c_str(), DenSupportedTypeToString(dt).c_str());
        KCTERR(ERR);
    }
    if(detectorTiltReader)
    {
        dt = detectorTiltReader->getDataType();
        if(detectorTiltReader->dimx() != 1 || detectorTiltReader->dimy() != 1)
        {
            ERR = io::xprintf("Dimensions of the file %s shall be dimx=1 and dimy=1 but they are "
                              "dimx=%d dimy=%d.",
                              detectorTiltFile.c_str(), detectorTiltReader->dimx(),
                              detectorTiltReader->dimy());
            KCTERR(ERR);
        }

        if(dt != io::DenSupportedType::FLOAT64)
        {
            ERR = io::xprintf("Element type of the file %s shall be double but it is %s!",
                              detectorTiltFile.c_str(), DenSupportedTypeToString(dt).c_str());
            KCTERR(ERR);
        }
    }
}

DenGeometry3DParallelReader::DenGeometry3DParallelReader(std::string projectionMatrixFile)
    : DenGeometry3DParallelReader(projectionMatrixFile, "")
{
}

DenGeometry3DParallelReader::DenGeometry3DParallelReader(std::string projectionMatrixFile,
                                                         std::string detectorTiltFile)
    : projectionMatrixFile(projectionMatrixFile)
    , detectorTiltFile(detectorTiltFile)
{
    matrixReader = std::make_shared<DenFrame2DReader<double>>(this->projectionMatrixFile);
    if(detectorTiltFile.empty())
    {
        detectorTiltReader = nullptr;
    } else
    {
        detectorTiltReader = std::make_shared<DenFrame2DReader<double>>(this->detectorTiltFile);
    }
    initObject();
}

geometry::Geometry3DParallel DenGeometry3DParallelReader::readGeometry(uint32_t k)
{
    std::array<double, 8> matrix;
    double offset;
    matrixReader->readFrameIntoBuffer(k, std::begin(matrix));
    if(detectorTiltReader)
    {
        matrixReader->readFrameIntoBuffer(k, &offset);
    } else
    {
        offset = 1.0;
    }
    geometry::Geometry3DParallelCameraMatrix m(std::begin(matrix));
    return geometry::Geometry3DParallel(m, offset);
}

uint32_t DenGeometry3DParallelReader::count() const { return geometriesCount; }

} // namespace KCT::io

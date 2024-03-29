#include "DEN/DenProjectionMatrixReader.hpp"

namespace KCT::io {
DenProjectionMatrixReader::DenProjectionMatrixReader(std::string projectionMatrixFile)
{
    this->projectionMatrixFile = projectionMatrixFile;
    DenFileInfo mi = DenFileInfo(this->projectionMatrixFile);
    this->offset = mi.getOffset();
    uint32_t cols, rows;
    cols = mi.getNumCols(); // Its matrix, dealing with strange data format considerations
    rows = mi.getNumRows(); // Its matrix, dealing with strange data format considerations
    countMatrices = mi.getNumSlices();
    DenSupportedType dt = mi.getElementType();
    std::string ERR;
    if(rows != 3 || cols != 4)
    {
        std::string ERR
            = io::xprintf("The file %s does not seem to contain projection matrices, since it has "
                          "dimensions (rows, cols) = (%d, %d) and should have (3, 4).",
                          this->projectionMatrixFile.c_str(), rows, cols);
        KCTERR(ERR);
    }
    if(dt != io::DenSupportedType::FLOAT64)
    {
        ERR = io::xprintf("Element type of the file %s shall be FLOAT64 but it is %s!",
                          projectionMatrixFile.c_str(), DenSupportedTypeToString(dt).c_str());
        KCTERR(ERR);
    }
}

matrix::ProjectionMatrix DenProjectionMatrixReader::readMatrix(int i)
{
    uint8_t buffer[8 * 3 * 4];
    uint64_t position = this->offset + ((uint64_t)i) * 3 * 4 * 8;
    io::readBytesFrom(this->projectionMatrixFile, position, buffer, 8 * 3 * 4);
    double matrixData[3 * 4];
    for(int a = 0; a != 3 * 4; a++)
    {
        matrixData[a] = util::nextDouble(&buffer[a * 8]);
    }
    return matrix::ProjectionMatrix(matrixData);
}

uint32_t DenProjectionMatrixReader::count() const { return countMatrices; }

} // namespace KCT::io

#pragma once
// External libraries

// Internal libraries
#include "MATRIX/ProjectionMatrix.hpp"

namespace CTL {
namespace io {
    /**
     *Interface for reading Frame2D objects.
     *
     *The implementation might be "slim implementation" that access underlying source each time it
     *is called. The implementation might be also "fat implementation" that holds in memory
     *structure that source each call.
     */
    class DenProjectionMatrixReader : ProjectionMatrixReaderI
    {
    private:
        int count;
        std::string projectionMatrixFile;

    public:
        DenProjectionMatrixReader(std::string projectionMatrixFile);

        /*Returns i-th projection slice in the source.*/
        util::ProjectionMatrix readMatrix(int i) override;
        /**Number of projection matrices in the source.*/
        unsigned int count() override;
    };

    DenProjectionMatrixReader::DenProjectionMatrixReader(std::string projectionMatrixFile)
    {
        this->projectionMatrixFile = projectionMatrixFile;
        DenFileInfo mi = DenFileInfo(this->projectionMatrixFile);
        int cols, rows;
        cols = mi.getNumCols(); // Its matrix, dealing with strange data format considerations
        rows = mi.getNumRows(); // Its matrix, dealing with strange data format considerations
        count = mi.getNumSlices();
        if(rows != 3 || cols != 4)
        {
            std::string errMsg = io::xprintf(
                "The file %s does not seem to contain projection matrices, since it has "
                "dimensions (rows, cols) = (%d, %d) and should have (3, 4).",
                this->projectionsFile.c_str(), rows, cols);
            LOGE << errMsg;
            throw std::runtime_error(errMsg);
        }
        if(mi.ElementByteSize() != 8)
        {
            io::throwerr(
                "The elemenst of the matrices are not doubles, I can not work with this format.");
        }
    }

    util::ProjectionMatrix DenProjectionMatrixReader::readMatrix(int i)
    {
        uint8_t buffer[8 * 3 * 4];
        uint64_t position = ((uint64_t)6) + ((uint64_t)sliceNum) * 3 * 4 * 8;
        io::readBytesFrom(this->projectionMatrixFile, position, buffer, 8 * 3 * 4);
        double matrixData[3 * 4];
        for(int a = 0; a != 3 * 4; a++)
        {
            matrixData[a] = util::nextDouble(&buffer[a * 8]);
        }
        return util::ProjectionMatrix(matrixData);
    }

} // namespace io
} // namespace CTL

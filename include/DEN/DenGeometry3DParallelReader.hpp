#pragma once
// External libraries

// Internal libraries
//#include "MATRIX/ProjectionMatrix.hpp"
#include "DEN/DenFileInfo.hpp"
#include "DEN/DenFrame2DReader.hpp"
#include "GEOMETRY/Geometry3DParallel.hpp"

namespace KCT::io {
/**
 * Interface for reading 3D parallel ray geometry.
 *
 * Geometry is represented by projection matrices and cos tilt scalars, in total trajectory of n
 * points is represented. projection matrices are 3D DEN files 2x4 with (dimx, dimy, dimz) =
 * (4,2,n), cos tilt scalars are 3D DEN files with (dimx, dimy, dimz) = (1,1,n).
 */
class DenGeometry3DParallelReader
{
private:
    std::string projectionMatrixFile;
    std::string detectorTiltFile;
    std::shared_ptr<DenFrame2DReader<double>> matrixReader = nullptr;
    std::shared_ptr<DenFrame2DReader<double>> detectorTiltReader = nullptr;
    uint32_t geometriesCount;
    void initObject();

public:
    /**
     * Creates reader with given projectionMatrixFile, assumes all the tilts are 1.0 that means
     * detector orthogonal to the incomming rays.
     *
     * @param projectionMatrixFile FLOAT64(4,2,count)
     */
    DenGeometry3DParallelReader(std::string projectionMatrixFile);
    /**
     * Creates reader with given projectionMatrixFile and detectorTilt file.
     *
     * @param projectionMatrixFile Den file FLOAT64(4,2,count).
     * @param detectorTiltFile Den file FLOAT64(1,1,count), if empty string is passed, behaves as if
     * the file would not be specified and assumes all the tilts being 1.0.
     */
    DenGeometry3DParallelReader(std::string projectionMatrixFile, std::string detectorTiltFile);

    /*Returns i-th geometry.*/
    geometry::Geometry3DParallel readGeometry(uint32_t k);
    /**Number of projection matrices in the source.*/
    uint32_t count() const;
};
} // namespace KCT::io

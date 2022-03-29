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

public:
    void initObject();
    DenGeometry3DParallelReader(std::string projectionMatrixFile);
    DenGeometry3DParallelReader(std::string projectionMatrixFile, std::string detectorTiltFile);

    /*Returns i-th geometry.*/
    geometry::Geometry3DParallel readGeometry(uint32_t k);
    /**Number of projection matrices in the source.*/
    uint32_t count() const;
};
} // namespace KCT::io

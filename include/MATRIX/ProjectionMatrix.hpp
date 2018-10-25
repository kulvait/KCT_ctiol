#pragma once
// Logging
#include <plog/Log.h>

// Standard libraries
#include <array>

// Internal libraries
#include "matrix.h"
#include "MATRIX/LUDoolittleForm.hpp"
#include "MATRIX/SquareMatrix.hpp"

namespace CTL {
namespace util {
    /**
     *Class to represent projection matrices.
     */
    class ProjectionMatrix : public Matrix<3, 4>
    {
    public:
        ProjectionMatrix()
            : Matrix<3, 4>(){};
        /**Constructs new ProjectionMatrix that is inicialized by zeros.*/
        ProjectionMatrix(const double (&initArray)[3 * 4])
            : Matrix<3, 4>(initArray)
        {
        }
        /**Constructor from the double array*/
        ProjectionMatrix(const Matrix<3, 4>& pm)
            : Matrix<3, 4>(pm)
        {
        }
        /**Constructor from the 3x4 Matrix.*/
        std::array<double, 3> sourcePosition();
        /**Get source position*/
        std::array<double, 3> normalToDetector();
        /**Get normal to detector ending at 0*/
        SquareMatrix<3> colSubMatrix(int i);
        /*Get 3x3 submatrix of projection matrix, where i-th row is removed.*/
    };
} // namespace util
} // namespace CTL

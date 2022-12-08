#pragma once

// External libraries
#include "itkImageFileReader.h"
#include "itkImageIOBase.h"
#include "itkRawImageIO.h"

// Internal libraries
#include "Frame2DI.hpp"
#include "MATRIX/ProjectionMatrix.hpp"
#include "ProjectionReaderI.hpp"

namespace KCT {
namespace io {
    /**
     *Interface for reading projections and related projection matrices in the format specified in
     *"Matrix-Template" project.
     *
     *The implementation might be "slim implementation" that access underlying source each time it
     *is called. The implementation might be also "fat implementation" that holds in memory
     *structure that source each call.
     */
    template <typename T>
    class ProjectionReaderItkI : virtual public ProjectionReaderI<T>
    // I don't want from implementer to implement all the behavior of ProjectionReaderI when its
    // implemented in some base class which is itself inherited
    {
    public:
        virtual typename itk::Image<T, 2>::Pointer readProjectionSliceAsItkImage(uint32_t i) = 0;
        /*Returns i-th projection slice in the source as itk::Image<T,2>.*/
        virtual ~ProjectionReaderItkI() = default;
    };
} // namespace io
} // namespace KCT

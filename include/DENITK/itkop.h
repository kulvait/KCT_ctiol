#pragma once

// External
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#include "itkCastImageFilter.h"
#include "itkImageFileWriter.h"
#include "itkIntensityWindowingImageFilter.h"
#include "itkRescaleIntensityImageFilter.h"
#pragma GCC diagnostic pop
#include <cstdlib> //EXIT_SUCCESS

// Internal
#include "Frame2DI.hpp"

namespace KCT {
namespace io {
    template <typename T>
    int writeImage(typename itk::Image<T, 2>::Pointer img, std::string outputFile);
    template <typename T>
    int writeCastedImage(typename itk::Image<T, 2>::Pointer img,
                         std::string outputFile,
                         double minValue = 0.0,
                         double maxValue = 0.0);
    template <typename T>
    int writeChannelsJpg(std::shared_ptr<Frame2DI<T>> img1,
                         std::shared_ptr<Frame2DI<T>> img2,
                         std::string outputFile);
    template <typename T>
    int writeChannelsRGB(std::shared_ptr<Frame2DI<T>> imgR,
                         std::shared_ptr<Frame2DI<T>> imgG,
                         std::shared_ptr<Frame2DI<T>> imgB,
                         std::string outputFile,
                         double minvalue = std::numeric_limits<double>::infinity(),
                         double maxvalue = -std::numeric_limits<double>::infinity());
} // namespace io
} // namespace KCT

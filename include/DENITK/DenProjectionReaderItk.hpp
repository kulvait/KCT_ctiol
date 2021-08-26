#pragma once

// External
#include "itkImageFileReader.h"
#include "itkImageIOBase.h"
#include "itkRawImageIO.h"
#include <string>

// Internal
#include "DEN/DenProjectionReader.hpp"
#include "DENITK/ProjectionReaderItkI.hpp"

namespace KCT {
namespace io {
    /**
     * Implementation of the ProjectionReader for projections and projection matrices stored in the
     * den files.
     */
    template <typename T>
    class DenProjectionReaderItk : public DenProjectionReader<T>, public ProjectionReaderItkI<T>
    // I need only one implementation of ProjectionReaderI<T> in my inheritance base, therefore I
    // will use virtual inheritance
    {
    public:
        DenProjectionReaderItk(std::string projectionsFile, std::string projectionMatrixFile);
        /**For the reader is necessery to provide file, where the projections are stored
         * (projectionsFile) and file where the projectionMatrices are stored
         * (projectionMatrixFile).
         *
         */
        typename itk::Image<T, 2>::Pointer readProjectionSliceAsItkImage(int i) override;
        /**Function to return projection slice as itk::Image<T, 2>*/
    };

    template <typename T>
    DenProjectionReaderItk<T>::DenProjectionReaderItk(std::string projectionsFile,
                                                      std::string projectionMatrixFile)
        : DenProjectionReader<T>(projectionsFile, projectionMatrixFile)
    {
    }

    template <typename T>
    typename itk::Image<T, 2>::Pointer
    DenProjectionReaderItk<T>::readProjectionSliceAsItkImage(int i)
    {
        //  LOGD << "Called readProjectionSliceToItkImage method, transpose???";
        typename itk::RawImageIO<T, 2>::Pointer rawImageIO = itk::RawImageIO<T, 2>::New();
        rawImageIO->SetFileName(this->projectionsFile); //(1) ... this is probably unnecessery
        rawImageIO->SetFileTypeToBinary();
        rawImageIO->SetHeaderSize(6 + i * this->elementByteSize * (this->sizex * this->sizey));
        rawImageIO->SetFileDimensionality(2);

        rawImageIO->SetOrigin(0, 0.0); // origin in millimeters
        rawImageIO->SetOrigin(1, 0.0);
        //	LOGD << io::xprintf("Setting dimensions of %s to (x,y) = (%d, %d)",
        // this->projectionsFile.c_str(), this->sizex, this->sizey);
        rawImageIO->SetDimensions(0, this->sizex); // size in pixels
        rawImageIO->SetDimensions(1, this->sizey);
        rawImageIO->SetSpacing(0, 1.0); // spacing in millimeters
        rawImageIO->SetSpacing(1, 1.0);

        rawImageIO->SetByteOrderToLittleEndian();
        rawImageIO->SetPixelType(itk::ImageIOBase::SCALAR);
        rawImageIO->SetNumberOfComponents(1);

        typename itk::ImageFileReader<itk::Image<T, 2>>::Pointer reader
            = itk::ImageFileReader<itk::Image<T, 2>>::New();
        reader->SetImageIO(rawImageIO);
        reader->SetFileName(this->projectionsFile); // Is it necessary when I have (1)
        reader->Update();
        return reader->GetOutput();
    }

} // namespace io
} // namespace KCT

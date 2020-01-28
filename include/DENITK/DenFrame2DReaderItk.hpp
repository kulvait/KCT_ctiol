#pragma once

// External
#include <string>

// Internal
#include "DENITK/Frame2DReaderItkI.hpp"

namespace CTL {
namespace io {
    /**
     * Chunk2D reader implementation that is also capable to read these chunks as ITK images.
     */
    template <typename T>
    class DenFrame2DReaderItk : public DenFrame2DReader<T>, public Frame2DReaderItkI<T>
    // Frame2DReaderI<T> will be only once in the family tree
    {
    public:
        /**Constructor from the denFile. Optionally is possible to set physical pixel spacing.
         *
         */
        DenFrame2DReaderItk(std::string denFile, float spX = 1.0, float spY = 1.0)
            : DenFrame2DReader<T>(denFile)
            , spacingX(spX)
            , spacingY(spY)
        {
        }
        /**DEN file to read chunks from.
         */
        typename itk::Image<T, 2>::Pointer readChunk2DAsItkImage(int i) override;
        /**Function to return projection slice as itk::Image<T, 2>*/
    private:
        float spacingX, spacingY;
    };

    template <typename T>
    typename itk::Image<T, 2>::Pointer DenFrame2DReaderItk<T>::readChunk2DAsItkImage(int i)
    {
        //  LOGD << "Called readProjectionSliceToItkImage method, transpose???";
        typename itk::RawImageIO<T, 2>::Pointer rawImageIO = itk::RawImageIO<T, 2>::New();
        rawImageIO->SetFileName(this->getFileName()); //(1) ... this is probably unnecessery
        rawImageIO->SetFileTypeToBinary();
        rawImageIO->SetHeaderSize(this->offset + i * this->elementByteSize * (this->sizex * this->sizey));
        rawImageIO->SetFileDimensionality(2);

        rawImageIO->SetOrigin(0, 0.0); // origin in millimeters
        rawImageIO->SetOrigin(1, 0.0);
        //  LOGD << io::xprintf("Setting dimensions of %s to (x,y) = (%d, %d)",
        //  this->projectionsFile.c_str(), this->sizex, this->sizey);
        rawImageIO->SetDimensions(0, this->sizex); // size in pixels
        rawImageIO->SetDimensions(1, this->sizey);
        rawImageIO->SetSpacing(0, spacingX); // spacing in millimeters
        rawImageIO->SetSpacing(1, spacingY);

        rawImageIO->SetByteOrderToLittleEndian();
        rawImageIO->SetPixelType(itk::ImageIOBase::SCALAR);
        rawImageIO->SetNumberOfComponents(1);

        typename itk::ImageFileReader<itk::Image<T, 2>>::Pointer reader
            = itk::ImageFileReader<itk::Image<T, 2>>::New();
        reader->SetImageIO(rawImageIO);
        reader->SetFileName(this->denFile); // Is it necessary when I have (1)
        reader->Update();
        return reader->GetOutput();
    }

} // namespace io
} // namespace CTL

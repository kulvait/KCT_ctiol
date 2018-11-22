#include "DENITK/itkop.h"
#include "frameop.h"

namespace CTL {
namespace io {

    template <typename T>
    int writeImage(typename itk::Image<T, 2>::Pointer img, std::string outputFile)
    {
        typename itk::ImageFileWriter<itk::Image<T, 2>>::Pointer writer
            = itk::ImageFileWriter<itk::Image<T, 2>>::New();
        writer->SetFileName(outputFile.c_str());
        writer->SetInput(img);
        writer->Update();
        return EXIT_SUCCESS;
    }

    template <typename T>
    int writeCastedImage(typename itk::Image<T, 2>::Pointer img,
                         std::string outputFile,
                         double minValue,
                         double maxValue)
    {
        using OutputPixelType = unsigned char;
        using OutputImageType = itk::Image<OutputPixelType, 2>;
        //using CastFilterType = itk::CastImageFilter<itk::Image<T, 2>, OutputImageType>;

        using WriterType = itk::ImageFileWriter<OutputImageType>;
        WriterType::Pointer writer = WriterType::New();

        using IntensityWindowingImageFilterType
            = itk::IntensityWindowingImageFilter<itk::Image<T, 2>, OutputImageType>;
        using RescalerType = itk::RescaleIntensityImageFilter<itk::Image<T, 2>, OutputImageType>;
        if(minValue == 0.0 && maxValue == 0.0)
        {
            typename RescalerType::Pointer intensityRescaler = RescalerType::New();
            intensityRescaler->SetInput(img);
            intensityRescaler->SetOutputMinimum(0);
            intensityRescaler->SetOutputMaximum(255);
            // typename CastFilterType::Pointer  caster =  CastFilterType::New();
            writer->SetFileName(outputFile);
            // caster->SetInput( img );
            writer->SetInput(intensityRescaler->GetOutput());
            writer->Update();
            return EXIT_SUCCESS;
        } else
        {
            typename IntensityWindowingImageFilterType::Pointer filter
                = IntensityWindowingImageFilterType::New();
            filter->SetInput(img);
            filter->SetWindowMinimum(minValue);
            filter->SetWindowMaximum(maxValue);
            filter->SetOutputMinimum(0);
            filter->SetOutputMaximum(255);
            // typename CastFilterType::Pointer  caster =  CastFilterType::New();
            writer->SetFileName(outputFile);
            // caster->SetInput( img );
            writer->SetInput(filter->GetOutput());
            writer->Update();
            return EXIT_SUCCESS;
        }
    }

    // This function returns value between 0-255 that interpolates between max and min
    uint8_t interpolateValue(double val, double min, double max)
    {
        double interpolation = 255.0 * (val - min) / (max - min);
        interpolation = interpolation + 0.5; // hack for rounding
        if(interpolation < 0.0)
        {
            interpolation = 0.0;
        }
        if(interpolation > 255.0)
        {
            interpolation = 255.0;
        }
        return (uint8_t)((int)interpolation);
    }

    template <typename T>
    int writeChannelsJpg(std::shared_ptr<Frame2DI<T>> img1,
                         std::shared_ptr<Frame2DI<T>> img2,
                         std::string outputFile)
    {
        double maxvalue, maxvalue1, maxvalue2;
        double minvalue, minvalue1, minvalue2;
        minvalue1 = minFrameValue<T>(*img1);
        maxvalue1 = maxFrameValue<T>(*img1);
        ;
        minvalue2 = minFrameValue<T>(*img2);
        maxvalue2 = maxFrameValue<T>(*img2);
        maxvalue = (maxvalue1 > maxvalue2 ? maxvalue1 : maxvalue2);
        minvalue = (minvalue1 < minvalue2 ? minvalue1 : minvalue2);
        int sizex, sizey;
        sizex = img1->dimx();
        sizey = img1->dimy();

        // Create itk::Image manually for resulting jpg
        using PixelType = itk::RGBPixel<unsigned char>;
        using ImageType = itk::Image<PixelType, 2>;
        ImageType::Pointer image = ImageType::New();
        ImageType::IndexType start;
        start[0] = 0; // Indices of the origin
        start[1] = 0;
        ImageType::SizeType size;
        size[0] = sizex; // size along X
        size[1] = sizey; // size along Y
        ImageType::RegionType region;
        region.SetSize(size);
        region.SetIndex(start);
        image->SetRegions(region);
        image->Allocate(); // Allocate memory for the image
        for(int i = 0; i != sizex; i++)
        {
            for(int j = 0; j != sizey; j++)
            {
                PixelType pixel;
                pixel[0] = interpolateValue(img1->get(i, j), minvalue, maxvalue);
                pixel[1] = interpolateValue(img2->get(i, j), minvalue, maxvalue);
                pixel[2] = 0;
                ImageType::IndexType pixelIndex = { { i, j } };
                image->SetPixel(pixelIndex, pixel);
            }
        }

        // Write created image
        using WriterType = itk::ImageFileWriter<ImageType>;
        WriterType::Pointer writer = WriterType::New();
        writer->SetFileName(outputFile);
        writer->SetInput(image);
        writer->Update();
        return EXIT_SUCCESS;
    }

    template <typename T>
    int writeChannelsRGB(std::shared_ptr<Frame2DI<T>> imgR,
                         std::shared_ptr<Frame2DI<T>> imgG,
                         std::shared_ptr<Frame2DI<T>> imgB,
                         std::string outputFile,
                         double minvalue,
                         double maxvalue)
    {
        int sizex, sizey;
        if(imgR != NULL)
        {
            sizex = imgR->dimx();
            sizey = imgR->dimy();
        }
        if(imgG != NULL)
        {
            sizex = imgG->dimx();
            sizey = imgG->dimy();
        }
        if(imgB != NULL)
        {
            sizex = imgB->dimx();
            sizey = imgB->dimy();
        }

        if(maxvalue == -std::numeric_limits<double>::infinity()
           && minvalue == std::numeric_limits<double>::infinity())
        {
            if(imgR != NULL)
            {
                double mintmp = (double)io::minFrameValue(*imgR);
                double maxtmp = (double)io::maxFrameValue(*imgR);
                minvalue = (minvalue < mintmp ? minvalue : mintmp);
                maxvalue = (maxvalue > maxtmp ? maxvalue : maxtmp);
            }
            if(imgG != NULL)
            {
                double mintmp = (double)minFrameValue(*imgG);
                double maxtmp = (double)maxFrameValue(*imgG);
                minvalue = (minvalue < mintmp ? minvalue : mintmp);
                maxvalue = (maxvalue > maxtmp ? maxvalue : maxtmp);
            }
            if(imgB != NULL)
            {
                double mintmp = (double)minFrameValue(*imgB);
                double maxtmp = (double)maxFrameValue(*imgB);
                minvalue = (minvalue < mintmp ? minvalue : mintmp);
                maxvalue = (maxvalue > maxtmp ? maxvalue : maxtmp);
            }
        }

        // Create itk::Image manually for resulting jpg
        using PixelType = itk::RGBPixel<unsigned char>;
        using ImageType = itk::Image<PixelType, 2>;
        ImageType::Pointer image = ImageType::New();
        ImageType::IndexType start;
        start[0] = 0; // Indices of the origin
        start[1] = 0;
        ImageType::SizeType size;
        size[0] = sizex; // size along X
        size[1] = sizey; // size along Y
        ImageType::RegionType region;
        region.SetSize(size);
        region.SetIndex(start);
        image->SetRegions(region);
        image->Allocate(); // Allocate memory for the image
        for(int i = 0; i != sizex; i++)
        {
            for(int j = 0; j != sizey; j++)
            {
                PixelType pixel;
                if(imgR != NULL)
                {
                    pixel[0] = interpolateValue(imgR->get(i, j), minvalue, maxvalue);
                } else
                {
                    pixel[0] = 0.0;
                }
                if(imgG != NULL)
                {
                    pixel[1] = interpolateValue(imgG->get(i, j), minvalue, maxvalue);
                } else
                {
                    pixel[1] = 0.0;
                }
                if(imgB != NULL)
                {
                    pixel[2] = interpolateValue(imgB->get(i, j), minvalue, maxvalue);
                } else
                {
                    pixel[2] = 0.0;
                }
                ImageType::IndexType pixelIndex = { { i, j } };
                image->SetPixel(pixelIndex, pixel);
            }
        }

        // Write created image
        using WriterType = itk::ImageFileWriter<ImageType>;
        WriterType::Pointer writer = WriterType::New();
        writer->SetFileName(outputFile);
        writer->SetInput(image);
        writer->Update();
        return EXIT_SUCCESS;
    }

    template int writeImage<float>(typename itk::Image<float, 2>::Pointer img,
                                   std::string outputFile);
    template int writeCastedImage<float>(typename itk::Image<float, 2>::Pointer img,
                                         std::string outputFile,
                                         double minValue,
                                         double maxValue);
    template int writeChannelsJpg<float>(std::shared_ptr<Frame2DI<float>> img1,
                                         std::shared_ptr<Frame2DI<float>> img2,
                                         std::string outputFile);
    template int writeChannelsRGB<float>(std::shared_ptr<Frame2DI<float>> imgR,
                                         std::shared_ptr<Frame2DI<float>> imgG,
                                         std::shared_ptr<Frame2DI<float>> imgB,
                                         std::string outputFile,
                                         double minvalue,
                                         double maxvalue);
} // namespace io
} // namespace CTL

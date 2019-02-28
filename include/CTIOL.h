#pragma once
/**All headers of the CTL::io*/
// Types without internal dependence
#include "DEN/DenSupportedType.hpp" //Data type used in DEN file, INCLUDES NOTHING

// Headers of functions
#include "stringFormatter.h" //Formating strings, INCLUDES NOTHING
// Ghost #include "utils/convertEndians.h" //includes io/DenSupportedType.hpp, io/stringFormatter.h
#include "rawop.h" //Raw reading of the part file into the uint8_t buffer Functions:readBytesFrom, includes from CTL: utils/convertEndians.h(io/DenSupportedType.hpp, io/stringFormatter.h)

// Classes
#include "DEN/DenFileInfo.hpp" //Provide information about particular den file, includes io/DenSupportedType.hpp, io/rawop.h, utils/convertEndians.h, io/DenSupportedType.hpp, io/stringFormatter.h
//#include "RawSliceReader.hpp" //Reading of the slices in the ITK format, depends on
//io/DenFileInfo.hpp, io/rawop.h, utils/convertEndians.h(io/DenSupportedType.hpp,
//io/stringFormatter.h)

// Interfaces
#include "DENITK/AsyncFrame2DWritterItkI.hpp" // Writing from itk images, INCLUDES NOTHING
// Impelementations
#include "DENITK/DICOMAsyncWritterItk.hpp" //Writing DICOM files, includes io/stringFormatter.h

#include "Frame2DI.hpp" // Template for the two dimensional array of templated type, INCLUDES NOTHING
// Implementations
#include "AsyncFrame2DWritterI.hpp" //Assynchronous image write, includes io/Frame2DI.hpp
#include "BufferedFrame2D.hpp" //INCLUDES io/Frame2DI.hpp
#include "DENITK/ItkImageChunk.hpp" //Provide interface to read ItkImage as a chunk, INCLUDES io/Frame2DI.hpp
// Implementations
#include "DEN/DenAsyncFrame2DWritter.hpp" //INCLUDES io/AsyncFrame2DWritterI.hpp

#include "Frame2DReaderI.hpp" //Reader for chunks, INCLUDES io/Frame2DI.hpp
// Implementations
#include "DEN/DenFrame2DReader.hpp" //INCLUDES io/Frame2DI.hpp, io/Frame2DReaderI.hpp, io/BufferedFrame2D.hpp
#include "DENITK/Frame2DReaderItkI.hpp" //Reader of chunks to itk images, INCLUDES io/Frame2DReaderI.hpp
// Implementations
#include "DENITK/DenFrame2DReaderItk.hpp" //Den files, INCLUDES io/Frame2DReaderItkI.hpp

// Ghost #include "utils/SquareMatrix.hpp"//INCLUDES NOTHING
// Ghost #include "utils/LUDoolittleForm.hpp"//includes utils/SquareMatrix.hpp, io/stringFormatter.h
// Ghost #include "utils/ProjectionMatrix.hpp"//includes utils/LUDoolittleForm.hpp,
// utils/SquareMatrix.hpp
#include "ProjectionReaderI.hpp" //Template for reading projections and projection matrices together, includes io/Frame2DI.hpp, utils/ProjectionMatrix.hpp
// Implementations
#include "DEN/DenProjectionReader.hpp" //Read den files, INCLUDES io/Frame2DI.hpp, io/BufferedFrame2D.hpp, utils/ProjectionMatrix.hpp
#include "DENITK/ProjectionReaderItkI.hpp" //Template for Reading projections extending ProjectionReader so that it is able to return itk::Image<T, 2>::Pointer objects from individual slices, includes "io/ProjectionReaderI.hpp, io/Frame2DI.hpp, utils/ProjectionMatrix.hpp
// Implementations
#include "DENITK/DenProjectionReaderItk.hpp" //Read den files to itk images, INCLUDES io/Frame2DI.hpp, io/BufferedFrame2D.hpp, utils/ProjectionMatrix.hpp, io/DenProjectionReader.hpp, io/ProjectionReaderItkI.hpp

// Headers of functions including something
#include "DENITK/itkop.h" //Writing images, INCLUDES io/Frame2DI.hpp
#include "frameop.h" //Statistics of frames, INCLUDES io/DenSupportedType.hpp, io/Frame2DI.hpp

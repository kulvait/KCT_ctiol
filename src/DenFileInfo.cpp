#include "DEN/DenFileInfo.hpp"

namespace CTL {
namespace io {
    /**Constructor*/
    DenFileInfo::DenFileInfo(std::string fileName)
        : fileName(fileName)
    {
    }

    /**X dimension*/
    uint16_t DenFileInfo::dimx() const
    {
        uint8_t buffer[2];
        readBytesFrom(this->fileName, 2, buffer, 2);
        return (util::nextUint16(buffer));
    }

    /**Y dimension*/
    uint16_t DenFileInfo::dimy() const
    {
        uint8_t buffer[2];
        readBytesFrom(this->fileName, 0, buffer, 2);
        return (util::nextUint16(buffer));
    }

    /**Z dimension*/
    uint16_t DenFileInfo::dimz() const
    {
        uint8_t buffer[4];
        readBytesFrom(this->fileName, 0, buffer, 2);
        return (util::nextUint16(buffer));
    }

    /**Y dimension*/
    uint16_t DenFileInfo::getNumRows() const { return this->dimy(); }

    /**X dimension*/
    uint16_t DenFileInfo::getNumCols() const { return this->dimx(); }

    /**Z dimension*/
    uint16_t DenFileInfo::getNumSlices() const { return this->dimz(); }

    /**File size
     */
    uint64_t DenFileInfo::getSize() const
    {
        std::ifstream ifs(this->fileName, std::ifstream::ate | std::ifstream::binary);
        uint64_t size = ifs.tellg();
        ifs.close();
        return size;
    }

    /**Total number of pixels as a product of dimensions
     */
    uint64_t DenFileInfo::getNumPixels() const
    {
        return uint64_t(this->dimx()) * uint64_t(this->dimy()) * uint64_t(this->dimz());
    }

    DenSupportedType DenFileInfo::getDataType() const
    {
        int elementByteSize = this->elementByteSize();
        switch(elementByteSize)
        {
        case 2:
            return DenSupportedType::uint16_t_;
        case 4:
            return DenSupportedType::float_;
        case 8:
            return DenSupportedType::double_;
        default:
            std::stringstream errMsg;
            errMsg << "File " << this->fileName
                   << " is not valid DEN file because it has datatype of the length "
                   << elementByteSize << ".";
            LOGE << errMsg.str();
            throw std::runtime_error(errMsg.str());
        }
    }

    uint8_t DenFileInfo::elementByteSize() const
    {
        uint64_t dataSize = this->getSize() - 6;
        uint64_t numPixels = this->getNumPixels();
        if(dataSize == 0)
        {
            if(numPixels == 0)
            {
                return 0;
            } else
            {
                std::stringstream errMsg;
                errMsg << "File " << this->fileName << " is not valid DEN file.";
                LOGE << errMsg.str();
                throw std::runtime_error(errMsg.str());
            }
        }
        if(dataSize < 0 || numPixels <= 0)
        {
            std::stringstream errMsg;
            errMsg
                << "File " << this->fileName
                << " is not valid DEN file because it is shorter than 6 bytes or number of pixels "
                   "is nonpositive.";
            LOGE << errMsg.str();
            throw std::runtime_error(errMsg.str());
        }
        if(dataSize % numPixels != 0)
        {
            std::stringstream errMsg;
            errMsg << "File " << this->fileName
                   << " is not valid DEN file because its data are not aligned with a pixels "
                      "represented.";
            LOGE << errMsg.str();
            throw std::runtime_error(errMsg.str());
        }
        switch(dataSize / numPixels)
        {
        case 2:
            return 2;
        case 4:
            return 4;
        case 8:
            return 8;
        default:
            std::stringstream errMsg;
            errMsg << "File " << this->fileName
                   << " is not valid DEN file because it has datatype of the length "
                   << dataSize / numPixels << ".";
            LOGE << errMsg.str();
            throw std::runtime_error(errMsg.str());
        }
    }

} // namespace io
} // namespace CTL

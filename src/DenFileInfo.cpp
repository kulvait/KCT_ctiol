#include "DEN/DenFileInfo.hpp"

namespace CTL {
namespace io {
    /**Constructor*/
    DenFileInfo::DenFileInfo(std::string fileName)
        : fileName(fileName)
    {
        std::string ERR;
        uint8_t buffer[6];
        readBytesFrom(this->fileName, 0, buffer, 6);
        uint32_t dimx, dimy, dimz;
        uint64_t byteSize;
        dimy = util::nextUint16(buffer);
        dimx = util::nextUint16(buffer + 2);
        dimz = util::nextUint16(buffer + 4);
        byteSize = getSize();
        // When dimx and dimy are 0 but Size is gt 6, we have extended format
        if(dimx == 0 && dimy == 0 && byteSize > 6)
        {
            if(dimz == 0)
            { // In extended format dimz==0 means X major format
                XMajorAlignment = true;
            } else if(dimz == 1)
            { // In extended format dimz==1 means Y major format
                XMajorAlignment = false;
            } else
            {
                ERR = io::xprintf("The file %s is not DEN nor DEN extended file as there is "
                                  "signature dimy=%d, dimx=%d, dimz=%d and size %d that is not "
                                  "valid combination.",
                                  fileName.c_str(), dimy, dimx, dimz, byteSize);
                LOGE << ERR;
                throw std::runtime_error(ERR);
            }
            uint8_t buffer[12];
            readBytesFrom(this->fileName, 6, buffer, 12);
            dimy = util::nextUint32(buffer);
            dimx = util::nextUint32(buffer + 4);
            dimz = util::nextUint32(buffer + 8);
            extended = true;
            offset = 18;
        } else
        {
            extended = false;
            XMajorAlignment = true;
            offset = 6;
        }
        byteSize -= offset;
        uint64_t elementNum = (uint64_t)dimx * dimy * dimz;
        if(elementNum * 2 != byteSize && elementNum * 4 != byteSize && elementNum * 8 != byteSize)
        {
            ERR = io::xprintf(
                "The file %s is not DEN nor DEN extended file as there is "
                "signature dimy=%d, dimx=%d, dimz=%d and size without header %d that is not "
                "valid combination.",
                fileName.c_str(), dimy, dimx, dimz, byteSize);
            LOGE << ERR;
            throw std::runtime_error(ERR);
        }
    }

    std::string DenFileInfo::getFileName() const { return this->fileName; }

    uint64_t DenFileInfo::getOffset() const { return offset; }
    bool DenFileInfo::isExtended() const { return extended; }
    bool DenFileInfo::hasXMajorAlignment() const { return XMajorAlignment; }
    bool DenFileInfo::isValid() const
    {
        uint64_t fileSize = this->getSize();
        uint64_t dataSize;
        if(fileSize >= offset)
        {
            dataSize = this->getSize() - offset;
        } else
        {
            return false;
        }
        uint64_t numPixels = this->getNumPixels();
        if(dataSize == 0 && numPixels == 0)
        {
            return true;
        }
        if(dataSize < numPixels || dataSize % numPixels != 0)
        {
            return false;
        }
        uint64_t elmSize = dataSize / numPixels;
        if(elmSize == 2 || elmSize == 4 || elmSize == 8)
        {
            return true;
        } else
        {
            return false;
        }
    }
    /**X dimension*/
    uint32_t DenFileInfo::dimx() const
    {
        if(extended)
        {
            uint8_t buffer[4];
            readBytesFrom(this->fileName, 6 + 4, buffer, 4);
            return (util::nextUint32(buffer));
        } else
        {
            uint8_t buffer[2];
            readBytesFrom(this->fileName, 2, buffer, 2);
            return (util::nextUint16(buffer));
        }
    }

    /**Y dimension*/
    uint32_t DenFileInfo::dimy() const
    {
        if(extended)
        {
            uint8_t buffer[4];
            readBytesFrom(this->fileName, 6, buffer, 4);
            return (util::nextUint32(buffer));
        } else
        {
            uint8_t buffer[2];
            readBytesFrom(this->fileName, 0, buffer, 2);
            return (util::nextUint16(buffer));
        }
    }

    /**Z dimension*/
    uint32_t DenFileInfo::dimz() const
    {
        if(extended)
        {
            uint8_t buffer[4];
            readBytesFrom(this->fileName, 6 + 8, buffer, 4);
            return (util::nextUint32(buffer));
        } else
        {
            uint8_t buffer[2];
            readBytesFrom(this->fileName, 4, buffer, 2);
            return (util::nextUint16(buffer));
        }
    }

    /**Y dimension*/
    uint32_t DenFileInfo::getNumRows() const { return this->dimy(); }

    /**X dimension*/
    uint32_t DenFileInfo::getNumCols() const { return this->dimx(); }

    /**Z dimension*/
    uint32_t DenFileInfo::getNumSlices() const { return this->dimz(); }

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
        std::string err;
        uint64_t fileSize = this->getSize();
        uint64_t dataSize;
        if(fileSize >= offset)
        {
            dataSize = this->getSize() - offset;
        } else
        {
            err = io::xprintf("File %s is not valid DEN file or extended DEN file, it does not "
                              "have even correctly defined offset!",
                              fileName.c_str());
            LOGE << err;
            throw std::runtime_error(err);
        }
        uint64_t numPixels = this->getNumPixels();
        if(dataSize == 0 && numPixels == 0)
        {
            return 0;
        }
        if(dataSize < numPixels || dataSize % numPixels != 0)
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

    void
    DenFileInfo::createDenHeader(std::string fileName, uint32_t dimx, uint32_t dimy, uint32_t dimz)
    {
        uint32_t uint16max = 65535;
        uint8_t buf[18];
        io::createEmptyFile(fileName, 0, true);
        if(dimx <= uint16max && dimy <= uint16max && dimz <= uint16max)
        {
            util::putUint16((uint16_t)dimy, &buf[0]);
            util::putUint16((uint16_t)dimx, &buf[2]);
            util::putUint16((uint16_t)dimz, &buf[4]);
            io::appendBytes(fileName, (uint8_t*)buf, (uint64_t)6);
        } else
        {
            util::putUint16(0, &buf[0]);
            util::putUint16(0, &buf[2]);
            util::putUint16(0, &buf[4]);
            util::putUint32(dimy, &buf[6]);
            util::putUint32(dimx, &buf[10]);
            util::putUint32(dimz, &buf[14]);
            io::appendBytes(fileName, (uint8_t*)buf, (uint64_t)18);
        }
    }

    void DenFileInfo::createDenExtendedHeader(std::string fileName,
                                                     uint32_t dimx,
                                                     uint32_t dimy,
                                                     uint32_t dimz,
                                                     bool XMajorAlignment)
    {
        uint8_t buf[18];
        io::createEmptyFile(fileName, 0, true);
        util::putUint16(0, &buf[0]);
        util::putUint16(0, &buf[2]);
        if(XMajorAlignment)//Default
        {
            util::putUint16(0, &buf[4]);
        } else
        {
            util::putUint16(0, &buf[4]);
        }
        util::putUint32(dimy, &buf[6]);
        util::putUint32(dimx, &buf[10]);
        util::putUint32(dimz, &buf[14]);
        io::appendBytes(fileName, (uint8_t*)buf, (uint64_t)18);
    }

} // namespace io
} // namespace CTL

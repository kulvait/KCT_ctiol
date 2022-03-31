#include "DEN/DenFileInfo.hpp"

namespace KCT {
namespace io {
    /**Constructor*/
    DenFileInfo::DenFileInfo(std::string fileName)
        : fileName(fileName)
    {
        std::string ERR;
        _fileSize = getSize();
        uint16_t h0, h1, h2, h3, h4;
        std::array<uint8_t, 74> buffer;
        readBytesFrom(this->fileName, 0, std::begin(buffer), 6);
        h0 = util::nextUint16(std::begin(buffer));
        h1 = util::nextUint16(std::begin(buffer) + 2);
        h2 = util::nextUint16(std::begin(buffer) + 4);
        if(h0 == 0 && _fileSize > 4095)
        {
            readBytesFrom(this->fileName, 0, std::begin(buffer), 74);
            h3 = util::nextUint16(std::begin(buffer) + 6);
            h4 = util::nextUint16(std::begin(buffer) + 8);
            extended = true;
            offset = 4096;
            if(h1 > 16)
            {
                KCTERR(io::xprintf("Maximum 16 dimensions is allowed by specification but h1=%d.",
                                   h1));
            } else
            {
                _dimCount = h1;
            }
            _elementByteSize = h2;
            if(h3 > 1)
            {
                KCTERR(io::xprintf("Values 0 or 1 are allowed by specification but h3=%d.", h3));
            }
            if(h3 == 0)
            {
                XMajorAlignment = true;
            } else
            {
                XMajorAlignment = false;
            }
            _elementType = getDenSupportedTypeByID(h4);
            uint32_t elementByteSize = DenSupportedTypeElementByteSize(_elementType);
            if(_elementByteSize != elementByteSize)
            {
                KCTERR(io::xprintf(
                    "Element byte size in file is %d but for the type %s it shall be %d.",
                    _elementByteSize, DenSupportedTypeToString(_elementType).c_str(),
                    elementByteSize));
            }
            for(uint32_t i = 0; i != _dimCount; i++)
            {
                _dim[i] = util::nextUint32(std::begin(buffer) + 10 + i * 4);
            }
        } else
        {
            extended = false;
            XMajorAlignment = true;
            offset = 6;
            _dimCount = 3;
            _dim[0] = h1;
            _dim[1] = h0;
            _dim[2] = h2;
            uint64_t elementCount = uint64_t(_dim[0]) * uint64_t(_dim[1]) * uint64_t(_dim[2]);
            uint64_t dataByteCount = _fileSize - offset;
            if(dataByteCount == elementCount * 2)
            {
                _elementType = DenSupportedType::UINT16;
            } else if(dataByteCount == elementCount * 4)
            {
                _elementType = DenSupportedType::FLOAT32;
            } else if(dataByteCount == elementCount * 8)
            {
                _elementType = DenSupportedType::FLOAT64;
            } else
            {
                KCTERR(io::xprintf("The file %s is not valid DEN file.", fileName.c_str()))
            }
        }
        _elementByteSize = DenSupportedTypeElementByteSize(_elementType);
        if(!isValid())
        {
            KCTERR(io::xprintf("The file %s is not valid DEN file.", fileName.c_str()))
        }
    }

    std::string DenFileInfo::getFileName() const { return this->fileName; }

    uint64_t DenFileInfo::getOffset() const { return offset; }
    bool DenFileInfo::isExtended() const { return extended; }
    bool DenFileInfo::hasXMajorAlignment() const { return XMajorAlignment; }
    bool DenFileInfo::isValid() const
    {
        uint64_t dataSize;
        if(_fileSize >= offset)
        {
            dataSize = _fileSize - offset;
        } else
        {
            return false;
        }
        uint64_t n = this->elementCount();
        if(n * _elementByteSize == dataSize)
        {
            return true;
        } else
        {
            return false;
        }
    }

    /**X dimension flat*/
    uint32_t DenFileInfo::dimx() const
    {
        if(_dimCount < 1)
        {
            return 0;
        } else
        {
            return _dim[0];
        }
    }

    /**Y dimension flat*/
    uint32_t DenFileInfo::dimy() const
    {
        if(_dimCount < 2)
        {
            return 1;
        } else
        {
            return _dim[1];
        }
    }

    /**Z dimension flat*/
    uint32_t DenFileInfo::dimz() const
    {
        const uint32_t UINT32_MAXXX = ((uint32_t)-1);
        if(_dimCount < 3)
        {
            return 1;
        } else if(_dimCount == 3)
        {
            return _dim[2];
        } else
        {
            uint64_t flatdimz = 1;
            for(int i = 2; i != _dimCount; i++)
            {
                flatdimz *= _dim[i];
            }
            if(flatdimz > UINT32_MAXXX)
            {
                KCTERR(io::xprintf("Flat dim z is not representable by uint32_t as it is %lu.",
                                   flatdimz));
            }
            return (uint32_t)flatdimz;
        }
    }

    /**Z dimension flat*/
    uint64_t DenFileInfo::dimflatz() const
    {
        if(_dimCount < 3)
        {
            return 1;
        } else if(_dimCount == 3)
        {
            return _dim[2];
        } else
        {
            uint64_t flatdimz = 1;
            for(int i = 2; i != _dimCount; i++)
            {
                flatdimz *= _dim[i];
            }
            return flatdimz;
        }
    }

    uint64_t DenFileInfo::frameSize() const
    {
        uint64_t _dimx = dimx();
        uint64_t _dimy = dimy();
        return _dimx * _dimy;
    }

    /**Y dimension*/
    uint32_t DenFileInfo::getNumRows() const { return this->dimy(); }

    /**X dimension*/
    uint32_t DenFileInfo::getNumCols() const { return this->dimx(); }

    /**Z dimension*/
    uint32_t DenFileInfo::getNumSlices() const { return this->dimz(); }

    uint32_t DenFileInfo::dim(uint32_t n) const
    {
        if(n < _dimCount)
        {

            return _dim[n];
        } else
        {
            KCTERR(io::xprintf("Dimension %d out of range of %d dimensional file %s.", n, _dimCount,
                               fileName.c_str()));
        }
    }

    uint16_t DenFileInfo::dimCount() const { return _dimCount; }

    uint64_t DenFileInfo::getSize() const
    {
        std::ifstream ifs(this->fileName, std::ifstream::ate | std::ifstream::binary);
        uint64_t size = ifs.tellg();
        ifs.close();
        return size;
    }

    /**Total number of elements as a product of dimensions
     */
    uint64_t DenFileInfo::elementCount() const
    {
        uint64_t n = 1;
        for(uint32_t i = 0; i != _dimCount; i++)
        {
            n *= uint64_t(_dim[i]);
        }
        return n;
    }

    DenSupportedType DenFileInfo::getDataType() const { return _elementType; }

    uint16_t DenFileInfo::elementByteSize() const { return _elementByteSize; }

    void DenFileInfo::createLegacyDenHeader(std::string fileName,
                                            uint16_t dimx,
                                            uint16_t dimy,
                                            uint16_t dimz)
    {
        // uint32_t uint16max = 65535;
        std::array<uint8_t, 6> buf;
        io::createEmptyFile(fileName, 0, true);
        util::putUint16((uint16_t)dimy, std::begin(buf));
        util::putUint16((uint16_t)dimx, std::begin(buf) + 2);
        util::putUint16((uint16_t)dimz, std::begin(buf) + 4);
        io::appendBytes(fileName, std::begin(buf), (uint64_t)6);
    }

    void DenFileInfo::create3DDenHeader(std::string fileName,
                                        DenSupportedType dst,
                                        uint32_t dimx,
                                        uint32_t dimy,
                                        uint32_t dimz,
                                        bool XMajorAlignment)
    {
        std::array<uint8_t, 4096> buf;
        util::putUint16(0, std::begin(buf));
        util::putUint16(3, std::begin(buf) + 2); // 3D
        util::putUint16(DenSupportedTypeElementByteSize(dst), std::begin(buf) + 4); // 3D
        if(XMajorAlignment) // Default
        {
            util::putUint16(0, std::begin(buf) + 6);
        } else
        {
            util::putUint16(1, std::begin(buf) + 6);
        }
        util::putUint16(DenSupportedTypeID(dst), std::begin(buf) + 8);
        util::putUint32(dimx, std::begin(buf) + 10);
        util::putUint32(dimy, std::begin(buf) + 14);
        util::putUint32(dimz, std::begin(buf) + 18);
        io::createEmptyFile(fileName, 0, true);
        io::appendBytes(fileName, std::begin(buf), (uint64_t)4096);
    }

    void DenFileInfo::createDenHeader(std::string fileName,
                                      DenSupportedType dst,
                                      uint16_t dimCount,
                                      uint32_t* dim,
                                      bool XMajorAlignment)
    {
        std::array<uint8_t, 4096> buf;
        util::putUint16(0, std::begin(buf));
        util::putUint16(3, std::begin(buf) + 2); // 3D
        util::putUint16(DenSupportedTypeElementByteSize(dst), std::begin(buf) + 4); // 3D
        if(XMajorAlignment) // Default
        {
            util::putUint16(0, std::begin(buf) + 6);
        } else
        {
            util::putUint16(1, std::begin(buf) + 6);
        }
        util::putUint16(DenSupportedTypeID(dst), std::begin(buf) + 8);
        for(uint16_t i = 0; i != dimCount; i++)
        {
            util::putUint32(dim[i], std::begin(buf) + 10 + 4 * i);
        }
        io::createEmptyFile(fileName, 0, true);
        io::appendBytes(fileName, std::begin(buf), (uint64_t)4096);
    }

    void DenFileInfo::createEmptyLegacyDenFile(
        std::string fileName, DenSupportedType dst, uint16_t dimx, uint16_t dimy, uint16_t dimz)
    {
        uint64_t totalElementCount = uint64_t(dimx) * uint64_t(dimy) * uint64_t(dimz);
        if(dst != DenSupportedType::UINT16 && dst != DenSupportedType::FLOAT32
           && dst != DenSupportedType::FLOAT64)
        {
            std::string ERR = io::xprintf("Legacy Den file can not have type %s.",
                                          DenSupportedTypeToString(dst).c_str());
            KCTERR(ERR);
        }
        uint64_t elementSize = DenSupportedTypeElementByteSize(dst);
        uint64_t totalFileSize = 6 + totalElementCount * elementSize;
        std::array<uint8_t, 6> buf;
        util::putUint16((uint16_t)dimy, std::begin(buf));
        util::putUint16((uint16_t)dimx, std::begin(buf) + 2);
        util::putUint16((uint16_t)dimz, std::begin(buf) + 4);
        io::createEmptyFile(fileName, totalFileSize, true);
        io::writeFirstBytes(fileName, std::begin(buf), 6);
    }

    void DenFileInfo::createEmpty3DDenFile(std::string fileName,
                                           DenSupportedType dst,
                                           uint32_t dimx,
                                           uint32_t dimy,
                                           uint32_t dimz,
                                           bool XMajorAlignment)
    {
        uint64_t totalElementCount = uint64_t(dimx) * uint64_t(dimy) * uint64_t(dimz);
        uint64_t elementSize = DenSupportedTypeElementByteSize(dst);
        uint64_t totalFileSize = 4096 + totalElementCount * elementSize;
        std::array<uint8_t, 4096> buf;
        util::putUint16(0, std::begin(buf));
        util::putUint16(3, std::begin(buf) + 2); // 3D
        util::putUint16(DenSupportedTypeElementByteSize(dst), std::begin(buf) + 4); // 3D
        if(XMajorAlignment) // Default
        {
            util::putUint16(0, std::begin(buf) + 6);
        } else
        {
            util::putUint16(1, std::begin(buf) + 6);
        }
        util::putUint16(DenSupportedTypeID(dst), std::begin(buf) + 8);
        util::putUint32(dimx, std::begin(buf) + 10);
        util::putUint32(dimy, std::begin(buf) + 14);
        util::putUint32(dimz, std::begin(buf) + 18);
        io::createEmptyFile(fileName, totalFileSize, true);
        io::writeFirstBytes(fileName, std::begin(buf), 4096);
    }

    void DenFileInfo::createEmptyDenFile(std::string fileName,
                                         DenSupportedType dst,
                                         uint16_t dimCount,
                                         uint32_t* dim,
                                         bool XMajorAlignment)
    {
        uint64_t totalElementCount = 1;
        if(dimCount == 0)
        {
            totalElementCount = 0;
        }
        for(uint16_t i = 0; i != dimCount; i++)
        {
            totalElementCount *= dim[i];
        }
        uint64_t elementSize = DenSupportedTypeElementByteSize(dst);
        uint64_t totalFileSize = 4096 + totalElementCount * elementSize;
        std::array<uint8_t, 4096> buf;
        util::putUint16(0, std::begin(buf));
        util::putUint16(3, std::begin(buf) + 2); // 3D
        util::putUint16(DenSupportedTypeElementByteSize(dst), std::begin(buf) + 4); // 3D
        if(XMajorAlignment) // Default
        {
            util::putUint16(0, std::begin(buf) + 6);
        } else
        {
            util::putUint16(1, std::begin(buf) + 6);
        }
        util::putUint16(DenSupportedTypeID(dst), std::begin(buf) + 8);
        for(uint16_t i = 0; i != dimCount; i++)
        {
            util::putUint32(dim[i], std::begin(buf) + 10 + 4 * i);
        }
        io::createEmptyFile(fileName, totalFileSize, true);
        io::writeFirstBytes(fileName, std::begin(buf), 4096);
    }

} // namespace io
} // namespace KCT

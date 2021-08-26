#pragma once

// External
#include <mutex>
#include <string>

// Internal
#include "BufferedFrame2D.hpp"
#include "DEN/DenFileInfo.hpp"
#include "DEN/DenSupportedType.hpp"
#include "Frame2DI.hpp"
#include "MATRIX/ProjectionMatrix.hpp"
#include "ProjectionReaderI.hpp"

namespace KCT {
namespace io {
    /**
     * Implementation of the ProjectionReader for projections and projection matrices stored in the
     * den files.
     */
    template <typename T>
    class DenProjectionReader : virtual public ProjectionReaderI<T>
    // ProjectionReaderI will be only once in the family tree
    {
    protected: // Visible in inheritance structure
        uint32_t sizex, sizey, sizez;
        std::string projectionsFile;
        std::string projectionMatrixFile;
        DenSupportedType dataType;
        uint32_t elementByteSize;
        uint64_t offsetMatrix = 6;
        uint64_t offsetProjections = 6;

    public:
        DenProjectionReader(std::string projectionsFile, std::string projectionMatrixFile);
        /**For the reader is necessery to provide file, where the projections are stored
         * (projectionsFile) and file where the projectionMatrices are stored
         * (projectionMatrixFile).
         *
         */
        std::shared_ptr<matrix::ProjectionMatrix> readProjectionMatrix(int i) override;
        std::shared_ptr<io::Frame2DI<T>> readProjectionSlice(int i) override;
        /**
         * Access to the slice of the DEN file that is ordered as in the file.
         */
        void* readNakedMemory(int sliceNum);
        uint32_t dimx() const override;
        uint32_t dimy() const override;
        uint32_t dimz() const override;

    private:
        mutable std::mutex matrixReadMutex;
    };

    template <typename T>
    DenProjectionReader<T>::DenProjectionReader(std::string projectionsFile,
                                                std::string projectionMatrixFile)
    {
        this->projectionsFile = projectionsFile;
        this->projectionMatrixFile = projectionMatrixFile;
        DenFileInfo pi = DenFileInfo(this->projectionsFile);
        DenFileInfo mi = DenFileInfo(this->projectionMatrixFile);
        this->sizex = pi.dimx();
        this->sizey = pi.dimy();
        this->sizez = pi.dimz();
        this->offsetMatrix = mi.getOffset();
        this->offsetProjections = mi.getOffset();
        uint32_t cols, rows, matCount;
        cols = mi.getNumCols(); // Its matrix, dealing with strange data format considerations
        rows = mi.getNumRows(); // Its matrix, dealing with strange data format considerations
        matCount = mi.getNumSlices();
        this->dataType = pi.getDataType();
        this->elementByteSize = pi.elementByteSize();
        if(rows != 3 || cols != 4)
        {
            std::string errMsg = io::xprintf(
                "The file %s does not seem to contain projection matrices, since it has "
                "dimensions (rows, cols) = (%d, %d) and should have (3, 4).",
                this->projectionsFile.c_str(), rows, cols);
            LOGE << errMsg;
            throw std::runtime_error(errMsg);
        }
        if(matCount != this->sizez)
        {
            std::string errMsg = io::xprintf(
                "There is a mismatch of the number of projections in file %s that is %d "
                "and the number of projections in the matrix file %s whidh is %d. Please "
                "check if the files are compatible.",
                this->projectionsFile.c_str(), this->sizez, this->projectionMatrixFile.c_str(),
                matCount);
            LOGE << errMsg;
            throw std::runtime_error(errMsg);
        }
    }

    template <typename T>
    unsigned int DenProjectionReader<T>::dimx() const
    {
        return sizex;
    }

    template <typename T>
    unsigned int DenProjectionReader<T>::dimy() const
    {
        return sizey;
    }

    template <typename T>
    unsigned int DenProjectionReader<T>::dimz() const
    {
        return sizez;
    }

    /**
    *This is de facto reimplementation of the following Python code
    def getProjMatrix(file, i):
            header = np.fromfile(file, np.dtype('<i2'), 3)
            if(header[0]!=3):
                    sys.exit('Number of rows should be 4')
            if(header[1]!=4):
                    sys.exit('Number of columns should be 3')
            f = open(file, "rb")
            f.seek(6+12*8*i, os.SEEK_SET)
            data = np.fromfile(f, np.dtype('<f8'), 12)
            newdata = data.reshape((3, 4))
            return(newdata)
                There was errors when multiple threads was reading the same part of memory.
    Therefore I place mutex here
    */
    template <typename T>
    std::shared_ptr<matrix::ProjectionMatrix>
    DenProjectionReader<T>::readProjectionMatrix(int sliceNum)
    {
        const std::lock_guard<std::mutex> lock(matrixReadMutex);
        uint8_t buffer[8 * 3 * 4];
        uint64_t position = this->offsetMatrix + ((uint64_t)sliceNum) * 3 * 4 * 8;
        io::readBytesFrom(this->projectionMatrixFile, position, buffer, 8 * 3 * 4);
        double matrixData[3 * 4];
        for(uint32_t a = 0; a != 3 * 4; a++)
        {
            matrixData[a] = util::nextDouble(&buffer[a * 8]);
        }
        std::shared_ptr<matrix::ProjectionMatrix> pm
            = std::make_shared<matrix::ProjectionMatrix>(matrixData);
        return pm;
    }

    template <typename T>
    std::shared_ptr<io::Frame2DI<T>> DenProjectionReader<T>::readProjectionSlice(int sliceNum)
    {
        uint8_t* buffer = new uint8_t[elementByteSize * sizex * sizey];
        uint64_t position
            = this->offsetProjections + ((uint64_t)sliceNum) * elementByteSize * sizex * sizey;
        io::readBytesFrom(this->projectionsFile, position, buffer, elementByteSize * sizex * sizey);
        T* buffer_copy = new T[sizex * sizey];
        for(uint32_t a = 0; a != sizex * sizey; a++)
        {
            buffer_copy[a] = util::getNextElement<T>(&buffer[a * elementByteSize], dataType);
        }
        std::shared_ptr<Frame2DI<T>> ps
            = std::make_shared<BufferedFrame2D<T>>(buffer_copy, sizex, sizey);
        delete[] buffer_copy; // Constructor creates new copy of the data
        delete[] buffer;
        return ps;
    }

    template <typename T>
    void* DenProjectionReader<T>::readNakedMemory(int sliceNum)
    {
        uint8_t* buffer = new uint8_t[elementByteSize * sizex * sizey];
        uint64_t position
            = this->offsetProjections + ((uint64_t)sliceNum) * elementByteSize * sizex * sizey;
        io::readBytesFrom(this->projectionsFile, position, buffer, elementByteSize * sizex * sizey);
        return buffer;
    }

} // namespace io
} // namespace KCT

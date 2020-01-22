#pragma once
// External
#include <mutex>
#include <string>

// Internal
#include "DEN/DenFileInfo.hpp"
#include "DEN/DenSupportedType.hpp"
#include "Frame2DI.hpp"
#include "FrameMemoryViewer2D.hpp"
#include "MATRIX/ProjectionMatrix.hpp"
#include "ProjectionReaderI.hpp"
#include "stringFormatter.h"

namespace CTL {
namespace io {
    /**
     * Implementation of the ProjectionReader for projections and projection matrices stored in the
     *den files.
     *
     *This class reads data into the naked memory array that is managed outside the class.
     *For synchronization, use readProjectionSliceIntoBufferView function.
     */
    template <typename T>
    class DenProjectionNakedReader : virtual public ProjectionReaderI<T>
    // ProjectionReaderI will be only once in the family tree
    {
    protected: // Visible in inheritance structure
        int sizex, sizey, sizez;
        std::string projectionsFile;
        std::string projectionMatrixFile;
        DenSupportedType dataType;
        int elementByteSize;
        mutable std::mutex writingMutex;

    public:
        /**For the reader is necessery to provide file, where the projections are stored
         * (projectionsFile) and file where the projectionMatrices are stored
         * (projectionMatrixFile).
         */
        DenProjectionNakedReader(std::string projectionsFile,
                                 std::string projectionMatrixFile,
                                 T* bufferToFill = NULL);

        /**Atomic reading into the buffer
         *
         * This function sets the position of the buffer to fill and then reads projection slice
         *into it. This is blocking operation so that this function is synchronized. Reads
         *projection slice into the memory on the address provided in T* buffer pointer and
         * returning an FrameMemoryViewer2D object.
         */
        std::shared_ptr<io::Frame2DI<T>> readProjectionSliceIntoBufferView(int i, T* buffer)
        {
            std::lock_guard<std::mutex> guard(writingMutex);
            setStartBuffer(buffer);
            return readProjectionSlice(i);
        }

        /**	Change starting buffer position.
         *
         */
        void setStartBuffer(T* buffer) { this->buffer = buffer; }

        std::shared_ptr<matrix::ProjectionMatrix> readProjectionMatrix(int i) override;
        /**Reads projection slice into the memory on the address provided in T* buffer pointer and
         * returning an FrameMemoryViewer2D object.
         *
         */
        std::shared_ptr<io::Frame2DI<T>> readProjectionSlice(int i) override;
        /**
         * Access to the slice of the DEN file that is ordered as in the file.
         */
        uint32_t dimx() const override;
        uint32_t dimy() const override;
        uint32_t dimz() const override;

        std::string getProjectionsFile() { return projectionsFile; }

    private:
        T* buffer;
		uint64_t offsetMatrix = 6;
		uint64_t offsetProjections = 6;
    };

    template <typename T>
    DenProjectionNakedReader<T>::DenProjectionNakedReader(std::string projectionsFile,
                                                          std::string projectionMatrixFile,
                                                          T* buffer)
    {
        this->projectionsFile = projectionsFile;
        this->projectionMatrixFile = projectionMatrixFile;
        DenFileInfo pi = DenFileInfo(this->projectionsFile);
        DenFileInfo mi = DenFileInfo(this->projectionMatrixFile);
        this->sizey = pi.getNumRows();
        this->sizex = pi.getNumCols();
        this->sizez = pi.getNumSlices();
		this->offsetMatrix = mi.getOffset();
		this->offsetProjections = mi.getOffset();
        int cols, rows, matCount;
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
            io::throwerr("There is a mismatch of the number of projections in file %s that is %d "
                         "and the number of projections in the matrix file %s whidh is %d. Please "
                         "check if the files are compatible.",
                         this->projectionsFile.c_str(), this->sizez,
                         this->projectionMatrixFile.c_str(), matCount);
        }
        this->buffer = buffer;
        if(buffer == NULL)
        {
            LOGW << io::xprintf("Allocating object with a NULL buffer. Please call the function "
                                "setStartBuffer before calling readProjectionSlice.");
        }
    }

    template <typename T>
    uint32_t DenProjectionNakedReader<T>::dimx() const
    {
        return sizex;
    }

    template <typename T>
    uint32_t DenProjectionNakedReader<T>::dimy() const
    {
        return sizey;
    }

    template <typename T>
    uint32_t DenProjectionNakedReader<T>::dimz() const
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
    */
    template <typename T>
    std::shared_ptr<matrix::ProjectionMatrix>
    DenProjectionNakedReader<T>::readProjectionMatrix(int sliceNum)
    {
        uint8_t buffer[8 * 3 * 4];
        uint64_t position = this->offsetMatrix + ((uint64_t)sliceNum) * 3 * 4 * 8;
        io::readBytesFrom(this->projectionMatrixFile, position, buffer, 8 * 3 * 4);
        double matrixData[3 * 4];
        for(int a = 0; a != 3 * 4; a++)
        {
            matrixData[a] = util::nextDouble(&buffer[a * 8]);
        }
        std::shared_ptr<matrix::ProjectionMatrix> pm
            = std::make_shared<matrix::ProjectionMatrix>(matrixData);
        return pm;
    }

    /** When using directly, this method is not synchronized.
     *
     */
    template <typename T>
    std::shared_ptr<io::Frame2DI<T>> DenProjectionNakedReader<T>::readProjectionSlice(int sliceNum)
    {
        uint64_t position = this->offsetProjections + uint64_t(sliceNum) * elementByteSize * sizex * sizey;
        uint8_t* tmp = new uint8_t[elementByteSize * sizex * sizey];
        io::readBytesFrom(this->projectionsFile, position, tmp, elementByteSize * sizex * sizey);
        for(int a = 0; a != sizex * sizey; a++)
        {
            buffer[a] = util::getNextElement<T>(&tmp[a * elementByteSize], dataType);
        }
        std::shared_ptr<Frame2DI<T>> ps
            = std::make_shared<FrameMemoryViewer2D<T>>(buffer, sizex, sizey);
        delete[] tmp;
        return ps;
    }

} // namespace io
} // namespace CTL

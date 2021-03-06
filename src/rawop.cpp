// Logging on the top
#include "rawop.h"

namespace KCT {
namespace io {

    void readFirstBytes(std::string fileName, uint8_t* buffer, int numBytes)
    {
        if(CHAR_BIT != 8)
        {
            std::stringstream errMsg;
            errMsg << "Can not use this platform since CHAR_BIT size is not 8, namely it is "
                   << CHAR_BIT << ".";
            LOGE << errMsg.str();
            throw std::runtime_error(errMsg.str());
        }
        std::ifstream file(fileName, std::ifstream::binary | std::ios::in); // binary for input
        if(!file.is_open()) // cannot open file
        {
            std::stringstream errMsg;
            errMsg << "Can not open file " << fileName << ".";
            LOGE << errMsg.str();
            throw std::runtime_error(errMsg.str());
        }

        file.read((char*)buffer, numBytes);
        auto num = file.gcount();
        file.close();
        if(num != numBytes)
        {
            std::stringstream errMsg;
            errMsg << "Can not read first " << numBytes << "bytes from the file " << fileName
                   << ".";
            LOGE << errMsg.str();
            throw std::runtime_error(errMsg.str());
        }
    }

    void readBytesFrom(std::string fileName,
                       uint64_t fromPosition,
                       uint8_t* buffer,
                       std::streamsize numBytes)
    {
        // LOGD << io::xprintf("Reading %d bytes from pos %lu.", numBytes, fromPosition);
        if(CHAR_BIT != 8)
        {
            std::stringstream errMsg;
            errMsg << "Can not use this platform since CHAR_BIT size is not 8, namely it is "
                   << CHAR_BIT << ".";
            LOGE << errMsg.str();
            throw std::runtime_error(errMsg.str());
        }
        std::ifstream file(fileName, std::ifstream::binary | std::ios::in); // binary for input
        if(!file.is_open()) // cannot open file
        {
            std::stringstream errMsg;
            errMsg << "Can not open file " << fileName << "failed with message: " << strerror(errno)
                   << ".";
            LOGE << errMsg.str();
            throw std::runtime_error(errMsg.str());
        }
        file.seekg(fromPosition);
        file.read((char*)buffer, numBytes);
        auto num = file.gcount();
        file.close();
        if(num != numBytes)
        {
            std::stringstream errMsg;
            errMsg << "Can not read " << numBytes << " bytes from the position " << fromPosition
                   << " in the file " << fileName << ".";
            LOGE << errMsg.str();
            throw std::runtime_error(errMsg.str());
        }
    }

    void writeFirstBytes(std::string fileName, uint8_t* buffer, uint64_t numBytes)
    {
        if(CHAR_BIT != 8)
        {
            std::string errMsg;
            errMsg = io::xprintf(
                "Can not use this platform since CHAR_BIT size is not 8, it is %d.", CHAR_BIT);
            KCTERR(errMsg);
        }
        std::ofstream file(fileName,
                           std::ios::binary | std::ios::out
                               | std::ios::in); // Open binary, for output, for input
        if(!file.is_open()) // cannot open file
        {
            std::string errMsg;
            errMsg = io::xprintf("Can not open file %s.", fileName.c_str());
            KCTERR(errMsg);
        }

        file.write((char*)buffer, numBytes);
        auto num = file.tellp();
        file.close();
        if(num != (int64_t)numBytes)
        {
            std::string errMsg;
            errMsg = io::xprintf("Can not read first %d bytes from file %s.", numBytes,
                                 fileName.c_str());
            KCTERR(errMsg);
        }
    }

    void
    writeBytesFrom(std::string fileName, uint64_t fromPosition, uint8_t* buffer, uint64_t numBytes)
    {
        if(CHAR_BIT != 8)
        {
            std::stringstream errMsg;
            errMsg << "Can not use this platform since CHAR_BIT size is not 8, namely it is "
                   << CHAR_BIT << ".";
            LOGE << errMsg.str();
            throw std::runtime_error(errMsg.str());
        }
        std::ofstream file(fileName,
                           std::ios::binary | std::ios::out
                               | std::ios::in); // Open binary, for output, for input
        if(!file.is_open()) // cannot open file
        {
            std::stringstream errMsg;
            errMsg << "Can not open file " << fileName << ".";
            LOGE << errMsg.str();
            throw std::runtime_error(errMsg.str());
        }
        file.seekp(fromPosition); // put pointer
        std::streampos cur = file.tellp();
        file.write((char*)buffer, numBytes);
        auto pos = file.tellp();
        auto num = pos - cur;
        file.close();
        if(num != (int64_t)numBytes)
        {
            std::stringstream errMsg;
            errMsg << num << " bytes written from number of bytess that should be written "
                   << numBytes << " to " << fileName << ".";
            LOGE << errMsg.str();
            throw std::runtime_error(errMsg.str());
        }
    }

    void appendBytes(std::string fileName, uint8_t* buffer, uint64_t numBytes)
    {
        if(CHAR_BIT != 8)
        {
            std::stringstream errMsg;
            errMsg << "Can not use this platform since CHAR_BIT size is not 8, namely it is "
                   << CHAR_BIT << ".";
            LOGE << errMsg.str();
            throw std::runtime_error(errMsg.str());
        }
        std::ofstream file(fileName,
                           std::ios::binary | std::ios::out | std::ios::in
                               | std::ios::ate); // Open binary, for output, for input
        if(!file.is_open()) // cannot open file
        {
            std::stringstream errMsg;
            errMsg << "Can not open file " << fileName << ".";
            LOGE << errMsg.str();
            throw std::runtime_error(errMsg.str());
        }
        std::streampos cur = file.tellp();
        file.write((char*)buffer, numBytes);
        auto pos = file.tellp();
        auto num = pos - cur;
        file.close();
        if(num != (int64_t)numBytes)
        {
            std::stringstream errMsg;
            errMsg << num << " bytes written from number of bytess that should be written "
                   << numBytes << " to " << fileName << ".";
            LOGE << errMsg.str();
            throw std::runtime_error(errMsg.str());
        }
    }

    bool pathExists(std::string fileName) { return (access(fileName.c_str(), F_OK) != -1); }

    bool isDirectory(const std::experimental::filesystem::path& p)
    {
        return std::experimental::filesystem::is_directory(p);
    }

    bool isRegularFile(const std::experimental::filesystem::path& p)
    {
        return std::experimental::filesystem::is_regular_file(p);
    }

    std::string getParent(const std::string& path)
    {
        std::string absolutePath = std::experimental::filesystem::absolute(path);
        // find last '/' or '\\' symbol in source string
        std::string::size_type found = absolutePath.find_last_of("/\\");
        if(found != std::string::npos)
        {
            return std::experimental::filesystem::canonical(absolutePath.substr(0, found));
        } else
        {
            return std::experimental::filesystem::canonical(".");
        }
    }

    std::string getBasename(const std::string& path)
    {
        // find last '/' or '\\' symbol in source string
        std::string::size_type found = path.find_last_of("/\\");
        // if we found one of this symbols
        if(found != std::string::npos)
        {
            return path.substr(found + 1);
        } else
        {
            return path;
        }
    }

    long getFileSize(std::string filename)
    {
        struct stat stat_buf;
        int rc = stat(filename.c_str(), &stat_buf);
        return rc == 0 ? stat_buf.st_size : -1;
    }

    void createEmptyFile(std::string fileName, uint64_t numBytes, bool overwrite)
    {
        if(pathExists(fileName))
        {
            if(overwrite)
            {
                std::remove(fileName.c_str());
                if(pathExists(fileName))
                {
                    std::string ERR = io::xprintf("Can not delete %f", fileName.c_str());
                    KCTERR(ERR);
                }
            } else
            {
                std::stringstream errMsg;
                errMsg << "Path " << fileName
                       << " already exist, set overwrite flag to overwritte.";
                KCTERR(errMsg.str());
                return;
            }
        }
        if(!pathExists(fileName) || overwrite)
        {
            std::ofstream ofs(
                fileName,
                std::ios::binary | std::ios::out
                    | std::ios::trunc); // Open binary, for output, truncate when opening
            if(numBytes > 0)
            {
                ofs.seekp(numBytes - 1);
                ofs.write("", 1);
            }
            ofs.close();
        }
    }

    /** File to string.
     *
     * This function converts file to string.
     * @param f File path.
     *
     * @return std::string that corresponds to the file contents.
     */
    std::string fileToString(const std::string& f)
    {
        if(!pathExists(f))
        {
            std::string ERR = io::xprintf("File %s does not exist.", f.c_str());
            KCTERR(ERR);
        }
        if(!isRegularFile(f))
        {
            std::string ERR = io::xprintf("File %s is not a regular file.", f.c_str());
            KCTERR(ERR);
        }
        std::ifstream ifs(f.c_str(), std::ios::in | std::ios::binary | std::ios::ate);

        std::ifstream::pos_type fileSize = ifs.tellg();
        ifs.seekg(0, std::ios::beg);

        std::vector<char> bytes(fileSize);
        ifs.read(bytes.data(), fileSize);

        return std::string(bytes.data(), fileSize);
    }

    void concatenateTextFiles(const std::string& outputFile,
                              bool overwrite,
                              std::initializer_list<std::string> inputFiles)
    {
        std::vector<std::string> inputFilesVector;
        for(std::string f : inputFiles)
        {
            inputFilesVector.push_back(f);
        }
        concatenateTextFiles(outputFile, overwrite, inputFilesVector);
    }

    void concatenateTextFiles(const std::string& outputFile,
                              bool overwrite,
                              std::vector<std::string>& inputFiles)
    {
        if(pathExists(outputFile) && !overwrite)
        {
            std::string ERR = io::xprintf("Ouptut file %s exists and overwrite is not set.",
                                          outputFile.c_str());
            KCTERR(ERR);
        }
        std::ofstream ofs(outputFile, std::ios_base::binary | std::ios::trunc);
        for(std::string f : inputFiles)
        {
            if(!isRegularFile(f))
            {
                std::string ERR = io::xprintf(
                    "File %s from the files to concatenate is not regular file!", f.c_str());
                // can throw std::ios_base::failure(ERR);
                KCTERR(ERR);
            }
            std::ifstream ifs(f, std::ios_base::binary);
            ofs << ifs.rdbuf();
            ifs.close();
            // See https://en.cppreference.com/w/cpp/io/basic_ios/fail
            if(ifs.fail())
            {
                KCTERR(io::xprintf("Failure reading %s.", f.c_str()));
            }
        }
        ofs.close();
        // See https://en.cppreference.com/w/cpp/io/basic_ios/fail
        if(ofs.fail())
        {
            KCTERR(io::xprintf("Failure writing %s.", outputFile.c_str()));
        }
    }
} // namespace io
} // namespace KCT

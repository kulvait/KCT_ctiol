// Logging on the top
#include "rawop.h"

namespace CTL {
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
            errMsg << "Can not open file " << fileName << ".";
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

        file.write((char*)buffer, numBytes);
        auto num = file.tellp();
        file.close();
        if(num != (int64_t)numBytes)
        {
            std::stringstream errMsg;
            errMsg << "Can not read first " << numBytes << "bytes from the file " << fileName
                   << ".";
            LOGE << errMsg.str();
            throw std::runtime_error(errMsg.str());
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

    bool fileExists(std::string fileName) { return (access(fileName.c_str(), F_OK) != -1); }

    std::string getParent(const std::string& path)
    {
	std::string absolutePath =  std::experimental::filesystem::canonical(path);
        // find last '/' or '\\' symbol in source string
        std::string::size_type found = absolutePath.find_last_of("/\\");
        if(found != std::string::npos)
        {
            return path.substr(0, found);
        } else
        {
            return "";
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
        if(!fileExists(fileName) || overwrite)
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
        } else
        {
            std::stringstream errMsg;
            errMsg << "File " << fileName
                   << " already exist, set overwrite flag if it should be overwritten.";
            LOGE << errMsg.str();
            throw std::runtime_error(errMsg.str());
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
        if(!fileExists(f))
        {
            std::string ERR = io::xprintf("File %s does not exist.", f.c_str());
            LOGE << ERR;
            io::throwerr(ERR);
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
        if(fileExists(outputFile) && !overwrite)
        {
            std::string ERR = io::xprintf("Ouptut file %s exists and overwrite is not set.",
                                          outputFile.c_str());
            LOGE << ERR;
            throw std::ios_base::failure(ERR);
        }
        std::ofstream ofs(outputFile, std::ios_base::binary | std::ios::trunc);
        for(std::string f : inputFiles)
        {
            if(!fileExists(f))
            {
                std::string ERR
                    = io::xprintf("One of the files to concatenate %s does not exist!", f.c_str());
                LOGE << ERR;
                throw std::ios_base::failure(ERR);
            }
            std::ifstream ifs(f, std::ios_base::binary);
            ofs << ifs.rdbuf();
            ifs.close();
        }
        ofs.close();
    }
} // namespace io
} // namespace CTL

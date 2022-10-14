// Logging on the top
#include "rawop.h"

namespace KCT {
namespace io {

    void readFirstBytes(std::string fileName, uint8_t* buffer, uint64_t numBytes)
    {
        std::string ERR;
        if(CHAR_BIT != 8)
        {
            ERR = io::xprintf("Can not use this platform since CHAR_BIT=%d is not 8!", CHAR_BIT);
            KCTERR(ERR);
        }
        std::ifstream file(fileName.c_str(),
                           std::ifstream::binary | std::ios::in); // binary for input
        if(!file.is_open()) // cannot open file
        {
#ifdef __linux__
            ERR = io::xprintf("Can not open file %s, strerror message :%s.", fileName.c_str(),
                              strerror(errno));
#else
            ERR = io::xprintf("Can not open file %s.", fileName.c_str());
#endif
            KCTERR(ERR);
        }

        file.read((char*)buffer, numBytes);
        auto num = file.gcount();
        file.close();
        if(num != (int64_t)numBytes)
        {
            ERR = io::xprintf("Can not read first %lu bytes from the file %s.", numBytes,
                              fileName.c_str());
            KCTERR(ERR);
        }
    }

    void
    readBytesFrom(std::string fileName, uint64_t fromPosition, uint8_t* buffer, uint64_t numBytes)
    {
        // LOGD << io::xprintf("Reading %d bytes from pos %lu.", numBytes, fromPosition);
        std::string ERR;
        if(CHAR_BIT != 8)
        {
            ERR = io::xprintf("Can not use this platform since CHAR_BIT=%d is not 8!", CHAR_BIT);
            KCTERR(ERR);
        }
        std::ifstream file(fileName.c_str(),
                           std::ios::binary | std::ios::in); // binary for input
        if(!file.is_open()) // cannot open file
        {
#ifdef __linux__
            ERR = io::xprintf("Can not open file %s, strerror message :%s.", fileName.c_str(),
                              strerror(errno));
#else
            ERR = io::xprintf("Can not open file %s.", fileName.c_str());
#endif
            KCTERR(ERR);
        }
        file.seekg(fromPosition);
        file.read((char*)buffer, numBytes);
        auto num = file.gcount();
        file.close();
        if(num != (int64_t)numBytes)
        {
            ERR = io::xprintf("Can not read first %lu bytes from the position %ld in file %s.",
                              numBytes, fromPosition, fileName.c_str());
            KCTERR(ERR);
        }
    }

    void
    readBytesFrom(std::shared_ptr<std::ifstream> ifstream, uint64_t fromPosition, uint8_t* buffer, uint64_t numBytes)
    {
        std::string ERR;
        if(!ifstream || !ifstream->is_open()) // cannot open file
        {
            ERR = "File descriptor is not open!";
            KCTERR(ERR);
        }
        std::streampos cur = ifstream->tellg();
        if(cur != (int64_t)fromPosition)
        {
            ifstream->seekg(fromPosition); // put pointer
            cur = ifstream->tellg();
        }
        ifstream->read((char*)buffer, numBytes);
        auto num = ifstream->gcount();
        if(num != (int64_t)numBytes)
        {
            ERR = io::xprintf("Can not read first %lu bytes from the position %ld in file descriptor.",
                              numBytes, fromPosition);
            KCTERR(ERR);
        }
    }

    void writeFirstBytes(std::string fileName, uint8_t* buffer, uint64_t numBytes)
    {
        std::string ERR;
        if(CHAR_BIT != 8)
        {
            ERR = io::xprintf("Can not use this platform since CHAR_BIT=%d is not 8!", CHAR_BIT);
            KCTERR(ERR);
        }
        std::ofstream file(fileName.c_str(),
                           std::ios::binary | std::ios::out
                               | std::ios::in); // Open binary, for output, for input
        if(!file.is_open()) // cannot open file
        {
#ifdef __linux__
            ERR = io::xprintf("Can not open file %s, strerror message :%s.", fileName.c_str(),
                              strerror(errno));
#else
            ERR = io::xprintf("Can not open file %s.", fileName.c_str());
#endif
            KCTERR(ERR);
        }
        file.write((char*)buffer, numBytes);
        auto num = file.tellp();
        file.close();
        if(num != (int64_t)numBytes)
        {
            ERR = io::xprintf("Can not write first %lu bytes to the file %s.", numBytes,
                              fileName.c_str());
            KCTERR(ERR);
        }
    }

    void writeBytesFrom(std::shared_ptr<std::ofstream> ofstream,
                        uint64_t fromPosition,
                        uint8_t* buffer,
                        uint64_t numBytes)
    {
        std::string ERR;
        if(!ofstream || !ofstream->is_open()) // cannot open file
        {
            ERR = "File descriptor is not open!";
            KCTERR(ERR);
        }
        std::streampos cur = ofstream->tellp();
        if(cur != (int64_t)fromPosition)
        {
            ofstream->seekp(fromPosition); // put pointer
            cur = ofstream->tellp();
        }
        ofstream->write((char*)buffer, numBytes);
        auto pos = ofstream->tellp();
        auto num = pos - cur;
        if(num != (int64_t)numBytes)
        {
            ERR = io::xprintf("Instead of %lu bytes to be written from position %lu to the ofstream"
                              "only %ld bytes was written.",
                              numBytes, fromPosition, num);
            KCTERR(ERR);
        }
    }

    void
    writeBytesFrom(std::string fileName, uint64_t fromPosition, uint8_t* buffer, uint64_t numBytes)
    {
        std::string ERR;
        if(CHAR_BIT != 8)
        {
            ERR = io::xprintf("Can not use this platform since CHAR_BIT=%d is not 8!", CHAR_BIT);
            KCTERR(ERR);
        }
        std::ofstream file(fileName.c_str(),
                           std::ios::binary | std::ios::out
                               | std::ios::in); // Open binary, for output, for input
        if(!file.is_open()) // cannot open file
        {
#ifdef __linux__
            ERR = io::xprintf("Can not open file %s, strerror message :%s.", fileName.c_str(),
                              strerror(errno));
#else
            ERR = io::xprintf("Can not open file %s.", fileName.c_str());
#endif
            KCTERR(ERR);
        }
        file.seekp(fromPosition); // put pointer
        std::streampos cur = file.tellp();
        file.write((char*)buffer, numBytes);
        auto pos = file.tellp();
        auto num = pos - cur;
        file.close();
        if(num != (int64_t)numBytes)
        {
            ERR = io::xprintf("Instead of %lu bytes to be written from position %lu to the file %s "
                              "only %ld bytes was written.",
                              numBytes, fromPosition, fileName.c_str(), num);
            KCTERR(ERR);
        }
    }

    void appendBytes(std::string fileName, uint8_t* buffer, uint64_t numBytes)
    {
        std::string ERR;
        if(CHAR_BIT != 8)
        {
            ERR = io::xprintf("Can not use this platform since CHAR_BIT=%d is not 8!", CHAR_BIT);
            KCTERR(ERR);
        }
        std::ofstream file(fileName.c_str(),
                           std::ios::binary | std::ios::out | std::ios::in
                               | std::ios::ate); // Open binary, for output, for input
        if(!file.is_open()) // cannot open file
        {
#ifdef __linux__
            ERR = io::xprintf("Can not open file %s, strerror message :%s.", fileName.c_str(),
                              strerror(errno));
#else
            ERR = io::xprintf("Can not open file %s.", fileName.c_str());
#endif
            KCTERR(ERR);
        }
        std::streampos cur = file.tellp();
        file.write((char*)buffer, numBytes);
        auto pos = file.tellp();
        auto num = pos - cur;
        file.close();
        if(num != (int64_t)numBytes)
        {
            ERR = io::xprintf("Instead of %lu bytes to be appended to the end of the file %s only "
                              "%ld bytes was written.",
                              numBytes, fileName.c_str(), num);
            KCTERR(ERR);
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
        std::string ERR;
        if(pathExists(fileName))
        {
            if(overwrite)
            {
                std::remove(fileName.c_str());
                if(pathExists(fileName))
                {
                    ERR = io::xprintf("Can not delete %f", fileName.c_str());
                    KCTERR(ERR);
                }
            } else
            {
                ERR = io::xprintf("Path %s already exists, to overwrite set overwrite flag.",
                                  fileName.c_str());
                KCTERR(ERR);
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
        std::string ERR;
        if(!pathExists(f))
        {
            ERR = io::xprintf("File %s does not exist.", f.c_str());
            KCTERR(ERR);
        }
        if(!isRegularFile(f))
        {
            ERR = io::xprintf("File %s is not a regular file.", f.c_str());
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
        std::string ERR;
        if(pathExists(outputFile) && !overwrite)
        {
            ERR = io::xprintf("Ouptut file %s exists and overwrite is not set.",
                              outputFile.c_str());
            KCTERR(ERR);
        }
        std::ofstream ofs(outputFile, std::ios_base::binary | std::ios::trunc);
        for(std::string f : inputFiles)
        {
            if(!isRegularFile(f))
            {
                ERR = io::xprintf("File %s from the files to concatenate is not regular file!",
                                  f.c_str());
                // can throw std::ios_base::failure(ERR);
                KCTERR(ERR);
            }
            std::ifstream ifs(f, std::ios_base::binary);
            ofs << ifs.rdbuf();
            ifs.close();
            // See https://en.cppreference.com/w/cpp/io/basic_ios/fail
            if(ifs.fail())
            {
                ERR = io::xprintf("Failure reading %s.", f.c_str());
                KCTERR(ERR);
            }
        }
        ofs.close();
        // See https://en.cppreference.com/w/cpp/io/basic_ios/fail
        if(ofs.fail())
        {
            ERR = io::xprintf("Failure writing %s.", outputFile.c_str());
            KCTERR(ERR);
        }
    }
} // namespace io
} // namespace KCT

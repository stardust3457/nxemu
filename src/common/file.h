#pragma once
#include <stdint.h>

struct IFile
{
public:
    enum OpenFlags
    {
        modeRead = 0x0000,
        modeWrite = 0x0001,
        modeReadWrite = 0x0002,
        shareCompat = 0x0000,
        shareExclusive = 0x0010,
        shareDenyWrite = 0x0020,
        shareDenyRead = 0x0030,
        shareDenyNone = 0x0040,
        modeNoInherit = 0x0080,
        modeCreate = 0x1000,
        modeNoTruncate = 0x2000,
    };

    enum class SeekPosition
    {
        begin = 0x0,
        current = 0x1,
        end = 0x2
    };

    virtual bool Open(const char * fileName, uint32_t openFlags) = 0;

    virtual uint64_t Seek(int64_t offset, SeekPosition from) = 0;
    virtual bool SetLength(uint64_t newLen) = 0;
    virtual uint64_t GetLength() const = 0;

    virtual uint32_t Read(void * buffer, uint32_t bufferSize) = 0;
    virtual bool Write(const void * buffer, uint32_t bufferSize) = 0;

    virtual bool Close() = 0;
    virtual bool IsOpen() const = 0;
    virtual bool SetEndOfFile() = 0;
};

class File :
    public IFile
{
public:
    File();
    File(void * fileHandle);
    File(const char * fileName, uint32_t openFlags);
    ~File();

    bool Open(const char * fileName, uint32_t openFlags);
    bool Close();

    uint64_t SeekToEnd(void);
    void SeekToBegin(void);

    uint64_t Seek(int64_t offset, SeekPosition from);
    bool SetLength(uint64_t newLen);
    uint64_t GetLength() const;

    uint32_t Read(void * buffer, uint32_t bufferSize);
    bool Write(const void * buffer, uint32_t bufferSize);

    bool IsOpen() const;
    bool SetEndOfFile();

private:
    File(const File &) = delete;
    File & operator=(const File &) = delete;

    void * m_file;
    bool m_closeOnDestroy;
};

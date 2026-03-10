#include "file.h"

#ifdef _WIN32
#define USE_WINDOWS_API
#include <Windows.h>
#else
#include <cstdint>
#include <cstring>
#include <errno.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>

#define INVALID_HANDLE_VALUE ((void *)(intptr_t)-1)

#endif

#if defined(_WIN32) && !defined(USE_WINDOWS_API)
#include <io.h>
#include <sys/locking.h>
#endif

#ifndef USE_WINDOWS_API
#include <fcntl.h>
#include <stdio.h>
#include <sys/stat.h>

static inline int HandleToFd(void * h)
{
    return (int)(intptr_t)h;
}
static inline void * FdToHandle(int fd)
{
    return (void *)(intptr_t)fd;
}
#endif

File::File() :
    m_file(INVALID_HANDLE_VALUE),
    m_closeOnDestroy(false)
{
}

File::File(void * fileHandle) :
    m_file(fileHandle),
    m_closeOnDestroy(true)
{
}

File::File(const char * fileName, uint32_t openFlags) :
    m_file(INVALID_HANDLE_VALUE),
    m_closeOnDestroy(true)
{
    Open(fileName, openFlags);
}

File::~File()
{
    if (m_file != INVALID_HANDLE_VALUE && m_closeOnDestroy)
    {
        Close();
    }
}

bool File::Open(const char * fileName, uint32_t openFlags)
{
    if (!Close())
    {
        return false;
    }

    if (fileName == nullptr || strlen(fileName) == 0)
    {
        return false;
    }

    m_file = INVALID_HANDLE_VALUE;

#ifdef USE_WINDOWS_API
    ULONG dwAccess = 0;
    switch (openFlags & 3)
    {
    case modeRead:
        dwAccess = GENERIC_READ;
        break;
    case modeWrite:
        dwAccess = GENERIC_WRITE;
        break;
    case modeReadWrite:
        dwAccess = GENERIC_READ | GENERIC_WRITE;
        break;
    default:
        return false;
    }

    ULONG shareMode = FILE_SHARE_READ | FILE_SHARE_WRITE;
    if ((openFlags & shareDenyWrite) == shareDenyWrite)
    {
        shareMode &= ~FILE_SHARE_WRITE;
    }
    if ((openFlags & shareDenyRead) == shareDenyRead)
    {
        shareMode &= ~FILE_SHARE_READ;
    }
    if ((openFlags & shareExclusive) == shareExclusive)
    {
        shareMode = 0;
    }

    SECURITY_ATTRIBUTES sa;
    sa.nLength = sizeof(sa);
    sa.lpSecurityDescriptor = nullptr;
    sa.bInheritHandle = (openFlags & modeNoInherit) == 0;

    ULONG createFlag = OPEN_EXISTING;
    if (openFlags & modeCreate)
    {
        createFlag = ((openFlags & modeNoTruncate) != 0) ? OPEN_ALWAYS : CREATE_ALWAYS;
    }

    HANDLE hFile = ::CreateFileA(fileName, dwAccess, shareMode, &sa, createFlag, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (hFile == INVALID_HANDLE_VALUE)
    {
        return false;
    }
    m_file = hFile;
#else
    int posixFlags = 0;
    switch (openFlags & 3)
    {
    case modeRead: posixFlags = O_RDONLY; break;
    case modeWrite: posixFlags = O_WRONLY; break;
    case modeReadWrite: posixFlags = O_RDWR; break;
    default: return false;
    }

    if (openFlags & modeCreate)
    {
        posixFlags |= O_CREAT;
        if ((openFlags & modeNoTruncate) == 0)
        {
            posixFlags |= O_TRUNC;
        }
    }

    int fd = -1;

#ifdef _WIN32
    if (openFlags & modeNoInherit)
    {
        posixFlags |= _O_NOINHERIT;
    }

    int shareFlag = _SH_DENYNO;
    if ((openFlags & shareExclusive) == shareExclusive)
    {
        shareFlag = _SH_DENYRW;
    }
    else if ((openFlags & shareDenyWrite) == shareDenyWrite)
    {
        shareFlag = _SH_DENYWR;
    }
    else if ((openFlags & shareDenyRead) == shareDenyRead)
    {
        shareFlag = _SH_DENYRD;
    }

    if (::_sopen_s(&fd, fileName, posixFlags, shareFlag, _S_IREAD | _S_IWRITE) != 0)
    {
        return false;
    }
#else
    if (openFlags & modeNoInherit)
    {
        posixFlags |= O_CLOEXEC;
    }

    fd = ::open(fileName, posixFlags, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
    if (fd < 0)
    {
        return false;
    }

    if ((openFlags & shareExclusive) == shareExclusive)
    {
        struct flock fl = {};
        fl.l_type = F_WRLCK;
        fl.l_whence = SEEK_SET;
        if (::fcntl(fd, F_SETLK, &fl) == -1)
        {
            ::close(fd);
            return false;
        }
    }
    else if ((openFlags & shareDenyWrite) == shareDenyWrite)
    {
        struct flock fl = {};
        fl.l_type = F_RDLCK;
        fl.l_whence = SEEK_SET;
        if (::fcntl(fd, F_SETLK, &fl) == -1)
        {
            ::close(fd);
            return false;
        }
    }
#endif

    m_file = FdToHandle(fd);
#endif

    m_closeOnDestroy = true;
    return true;
}

bool File::Close()
{
    bool bError = true;
    if (m_file != INVALID_HANDLE_VALUE)
    {
#ifdef USE_WINDOWS_API
        bError = !::CloseHandle(m_file);
#elif defined(_WIN32)
        bError = (::_close(HandleToFd(m_file)) != 0);
#else
        bError = (::close(HandleToFd(m_file)) != 0);
#endif
    }
    m_file = INVALID_HANDLE_VALUE;
    m_closeOnDestroy = false;
    return bError;
}

bool File::IsOpen(void) const
{
    return m_file != INVALID_HANDLE_VALUE;
}

void File::SeekToBegin(void)
{
    Seek(0, SeekPosition::begin);
}

uint64_t File::Seek(int64_t offset, SeekPosition from)
{
#ifdef USE_WINDOWS_API
    LONG distanceToMove = (LONG)(offset & 0xFFFFFFFF);
    LONG distanceToMoveHigh = (LONG)(offset >> 32);
    ULONG dwNew = ::SetFilePointer(m_file, distanceToMove, &distanceToMoveHigh, (ULONG)from);
    if (dwNew == (ULONG)-1)
    {
        return (uint64_t)-1;
    }
    return ((uint64_t)distanceToMoveHigh << 32) | dwNew;
#else
    int whence;
    switch (from)
    {
    case SeekPosition::begin: whence = SEEK_SET; break;
    case SeekPosition::current: whence = SEEK_CUR; break;
    case SeekPosition::end: whence = SEEK_END; break;
    default: return (uint64_t)-1;
    }
#ifdef _WIN32
    int64_t result = ::_lseeki64(HandleToFd(m_file), offset, whence);
#else
    int64_t result = (int64_t)::lseek(HandleToFd(m_file), (off_t)offset, whence);
#endif
    if (result == -1)
    {
        return (uint64_t)-1;
    }
    return (uint64_t)result;
#endif
}

bool File::SetLength(uint64_t dwNewLen)
{
    Seek(dwNewLen, SeekPosition::begin);
    return SetEndOfFile();
}

uint64_t File::GetLength() const
{
#ifdef USE_WINDOWS_API
    DWORD hiWord = 0;
    DWORD loWord = GetFileSize(m_file, &hiWord);
    return ((uint64_t)hiWord << 32) | (uint64_t)loWord;
#elif defined(_WIN32)
    struct _stat64 st;
    if (::_fstat64(HandleToFd(m_file), &st) != 0)
    {
        return (uint64_t)-1;
    }
    return (uint64_t)st.st_size;
#else
    struct stat st;
    if (::fstat(HandleToFd(m_file), &st) != 0)
    {
        return (uint64_t)-1;
    }
    return (uint64_t)st.st_size;
#endif
}

bool File::SetEndOfFile()
{
#ifdef USE_WINDOWS_API
    return ::SetEndOfFile(m_file) != 0;
#elif defined(_WIN32)
    int fd = HandleToFd(m_file);
    int64_t pos = ::_lseeki64(fd, 0, SEEK_CUR);
    if (pos == -1)
    {
        return false;
    }
    return ::_chsize_s(fd, pos) == 0;
#else
    int fd = HandleToFd(m_file);
    off_t pos = ::lseek(fd, 0, SEEK_CUR);
    if (pos == (off_t)-1)
    {
        return false;
    }
    return ::ftruncate(fd, pos) == 0;
#endif
}

uint32_t File::Read(void * lpBuf, uint32_t nCount)
{
    if (nCount == 0)
    {
        return 0;
    }

#ifdef USE_WINDOWS_API
    DWORD read = 0;
    if (!::ReadFile(m_file, lpBuf, nCount, &read, nullptr))
    {
        return 0;
    }
    return (uint32_t)read;
#elif defined(_WIN32)
    int result = ::_read(HandleToFd(m_file), lpBuf, nCount);
    if (result < 0)
    {
        return 0;
    }
    return (uint32_t)result;
#else
    int result = ::read(HandleToFd(m_file), lpBuf, nCount);
    if (result < 0)
    {
        return 0;
    }
    return (uint32_t)result;
#endif
}

bool File::Write(const void * buffer, uint32_t bufferSize)
{
    if (bufferSize == 0)
    {
        return true;
    }

#ifdef USE_WINDOWS_API
    ULONG written = 0;
    if (!::WriteFile(m_file, buffer, bufferSize, &written, nullptr))
    {
        return false;
    }
    if (written != bufferSize)
    {
        return false;
    }
#elif defined(_WIN32)
    int written = ::_write(HandleToFd(m_file), buffer, bufferSize);
    if (written < 0 || (uint32_t)written != bufferSize)
    {
        return false;
    }
#else
    int written = ::write(HandleToFd(m_file), buffer, bufferSize);
    if (written < 0 || (uint32_t)written != bufferSize)
    {
        return false;
    }
#endif

    return true;
}

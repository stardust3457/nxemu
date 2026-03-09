#include "path_finder.h"
#include "std_string.h"

#ifdef _WIN32
#include <Windows.h>
#else
#include <dirent.h>
#include <fnmatch.h>
#include <string.h>
#include <sys/stat.h>
#endif

#ifndef _WIN32
static const char DIR_SEPARATOR = '/';
#endif

PathFinder::PathFinder(const Path & searchSpec) :
    m_searchSpec(searchSpec),
#ifdef _WIN32
    m_findHandle(nullptr),
#else
    m_findDir(nullptr),
#endif
    m_findAttributes(0)
{
}

PathFinder::PathFinder(const std::string & searchSpec) :
    m_searchSpec(searchSpec),
#ifdef _WIN32
    m_findHandle(nullptr),
#else
    m_findDir(nullptr),
#endif
    m_findAttributes(0)
{
}

PathFinder::PathFinder(const char * searchSpec) :
    m_searchSpec(searchSpec),
#ifdef _WIN32
    m_findHandle(nullptr),
#else
    m_findDir(nullptr),
#endif
    m_findAttributes(0)
{
}

PathFinder::~PathFinder()
{
    CloseFindHandle();
}

bool PathFinder::FindFirst(Path & result, uint32_t attributes)
{
    CloseFindHandle();

    m_findAttributes = attributes;
    m_basePath = m_searchSpec.GetDriveDirectory();

#ifdef _WIN32
    WIN32_FIND_DATA findData;
    m_findHandle = FindFirstFile(stdstr((const char *)m_searchSpec).ToUTF16().c_str(), &findData);
    if (m_findHandle == INVALID_HANDLE_VALUE)
    {
        m_findHandle = nullptr;
        return false;
    }
    bool bWantSubdirectory = (FIND_ATTRIBUTE_SUBDIR & attributes) != 0;
    do
    {
        if (AttributesMatch(m_findAttributes, findData.dwFileAttributes) &&
            (!bWantSubdirectory || (findData.cFileName[0] != '.')))
        {
            std::string fileName = stdstr().FromUTF16(findData.cFileName);
            if ((FIND_ATTRIBUTE_SUBDIR & findData.dwFileAttributes) != 0)
            {
                result = Path(m_basePath, "");
                result.AppendDirectory(fileName.c_str());
            }
            else
            {
                result = Path(m_basePath, fileName.c_str());
            }
            return true;
        }
    } while (FindNextFile(m_findHandle, &findData));
    return false;
#else
    std::string specStr = (const char *)m_searchSpec;
    std::string::size_type lastSep = specStr.rfind(DIR_SEPARATOR);
    std::string dir;
    if (lastSep != std::string::npos)
    {
        dir = specStr.substr(0, lastSep);
        m_findPattern = specStr.substr(lastSep + 1);
    }
    else
    {
        dir = ".";
        m_findPattern = specStr;
    }

    if (dir.empty())
    {
        dir = "/";
    }

    m_findDir = opendir(dir.c_str());
    if (m_findDir == nullptr)
    {
        return false;
    }

    if (m_basePath.empty())
    {
        m_basePath = dir;
    }

    return MatchNext(result);
#endif
}

bool PathFinder::FindNext(Path & result)
{
#ifdef _WIN32
    if (m_findHandle == nullptr)
    {
        return false;
    }

    WIN32_FIND_DATA findData;
    while (FindNextFile(m_findHandle, &findData) != false)
    {
        if (AttributesMatch(m_findAttributes, findData.dwFileAttributes))
        {
            std::string fileName = stdstr().FromUTF16(findData.cFileName);
            if ((FIND_ATTRIBUTE_SUBDIR & findData.dwFileAttributes) == FIND_ATTRIBUTE_SUBDIR)
            {
                result = Path(m_basePath.c_str(), "");
                result.AppendDirectory(fileName.c_str());
            }
            else
            {
                result = Path(m_basePath, fileName.c_str());
            }
            return true;
        }
    }
    return false;
#else
    return MatchNext(result);
#endif
}

#ifndef _WIN32
bool PathFinder::MatchNext(Path & result)
{
    if (m_findDir == nullptr)
    {
        return false;
    }

    bool bWantSubdirectory = (FIND_ATTRIBUTE_SUBDIR & m_findAttributes) != 0;
    struct dirent * entry;

    while ((entry = readdir(m_findDir)) != nullptr)
    {
        // Skip . and ..
        if (entry->d_name[0] == '.')
        {
            if (entry->d_name[1] == '\0' || (entry->d_name[1] == '.' && entry->d_name[2] == '\0'))
            {
                continue;
            }
            // For subdirectory searches, skip all dot-entries
            if (bWantSubdirectory)
            {
                continue;
            }
        }

        // Match the filename pattern
        if (!m_findPattern.empty() && m_findPattern != "*" && m_findPattern != "*.*")
        {
            if (fnmatch(m_findPattern.c_str(), entry->d_name, 0) != 0)
            {
                continue;
            }
        }

        // Determine if this entry is a directory
        std::string fullEntryPath = m_basePath;
        if (!fullEntryPath.empty() && fullEntryPath.back() != DIR_SEPARATOR)
        {
            fullEntryPath += DIR_SEPARATOR;
        }
        fullEntryPath += entry->d_name;

        struct stat st;
        uint32_t entryAttributes = 0;
        if (stat(fullEntryPath.c_str(), &st) == 0 && S_ISDIR(st.st_mode))
        {
            entryAttributes = FIND_ATTRIBUTE_SUBDIR;
        }

        if (AttributesMatch(m_findAttributes, entryAttributes))
        {
            if ((FIND_ATTRIBUTE_SUBDIR & entryAttributes) == FIND_ATTRIBUTE_SUBDIR)
            {
                result = Path(m_basePath.c_str(), "");
                result.AppendDirectory(entry->d_name);
            }
            else
            {
                result = Path(m_basePath.c_str(), entry->d_name);
            }
            return true;
        }
    }
    return false;
}
#endif

bool PathFinder::AttributesMatch(uint32_t targetAttributes, uint32_t fileAttributes)
{
    if (targetAttributes == FIND_ATTRIBUTE_ALLFILES)
    {
        return true;
    }
    if (targetAttributes == FIND_ATTRIBUTE_FILES)
    {
        return ((FIND_ATTRIBUTE_SUBDIR & fileAttributes) == 0);
    }
    return (((targetAttributes & fileAttributes) != 0) && ((FIND_ATTRIBUTE_SUBDIR & targetAttributes) == (FIND_ATTRIBUTE_SUBDIR & fileAttributes)));
}

void PathFinder::CloseFindHandle()
{
#ifdef _WIN32
    if (m_findHandle != nullptr)
    {
        FindClose(m_findHandle);
        m_findHandle = nullptr;
    }
#else
    if (m_findDir != nullptr)
    {
        closedir(m_findDir);
        m_findDir = nullptr;
    }
#endif 
}

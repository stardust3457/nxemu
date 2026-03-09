#pragma once
#include "path.h"
#include <stdint.h>
#include <string>

class PathFinder
{
public:
    enum
    {
        FIND_ATTRIBUTE_ALLFILES = 0xFFFF, // Search include all files
        FIND_ATTRIBUTE_FILES = 0x0000,    // File can be read or written to without restriction
        FIND_ATTRIBUTE_SUBDIR = 0x0010,   // Subdirectories
    };

    PathFinder(const Path & searchSpec);
    PathFinder(const std::string & searchSpec);
    PathFinder(const char * searchSpec);
    ~PathFinder();

    bool FindFirst(Path & result, uint32_t attributes = 0);
    bool FindNext(Path & result);

private:
    PathFinder() = delete;
    PathFinder(const PathFinder &) = delete;
    PathFinder & operator=(const PathFinder &) = delete;

#ifndef _WIN32
    bool MatchNext(Path & result);
#endif

    bool AttributesMatch(uint32_t targetAttributes, uint32_t fileAttributes);
    void CloseFindHandle();

    Path m_searchSpec;
    std::string m_basePath;
#ifdef WIN32
    void * m_findHandle;
#else
    void * m_findDir = nullptr;
    std::string m_findPattern;
#endif
    uint32_t m_findAttributes;
};

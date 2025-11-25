#include "vfs_types.h"
#include "core/file_sys/romfs.h"
#include <yuzu_common/yuzu_assert.h>

VirtualDirectoryPtr::VirtualDirectoryPtr(FileSys::VirtualDir & directory) :
    m_directory(directory)
{
}

VirtualDirectoryPtr::~VirtualDirectoryPtr()
{
}

IVirtualDirectory * VirtualDirectoryPtr::CreateSubdirectory(const char * path) const
{
    if (m_directory.get() == nullptr)
    {
        return nullptr;
    }
    FileSys::VirtualDir dir = m_directory->CreateSubdirectory(path);
    if (dir.get() == nullptr)
    {
        return nullptr;
    }
    return std::make_unique<VirtualDirectoryPtr>(dir).release();
}

IVirtualDirectory * VirtualDirectoryPtr::GetDirectoryRelative(const char * path) const
{
    if (m_directory.get() == nullptr)
    {
        return nullptr;
    }
    FileSys::VirtualDir dir = m_directory->GetDirectoryRelative(path);
    if (dir.get() == nullptr)
    {
        return nullptr;
    }
    return std::make_unique<VirtualDirectoryPtr>(dir).release();
}

IVirtualDirectory * VirtualDirectoryPtr::GetSubdirectory(const char* path) const
{
    if (m_directory.get() == nullptr)
    {
        return nullptr;
    }
    FileSys::VirtualDir dir = m_directory->GetSubdirectory(path);
    if (dir.get() == nullptr)
    {
        return nullptr;
    }
    return std::make_unique<VirtualDirectoryPtr>(dir).release();
}

IVirtualFile * VirtualDirectoryPtr::CreateFile(const char * name) const
{
    if (m_directory.get() == nullptr)
    {
        return nullptr;
    }
    FileSys::VirtualFile file = m_directory->CreateFile(name);
    if (file.get() == nullptr)
    {
        return nullptr;
    }
    return std::make_unique<VirtualFilePtr>(file).release();
}

IVirtualFile * VirtualDirectoryPtr::GetFile(const char * name) const
{
    if (m_directory.get() == nullptr)
    {
        return nullptr;
    }
    FileSys::VirtualFile file = m_directory->GetFile(name);
    if (file.get() == nullptr)
    {
        return nullptr;
    }
    return std::make_unique<VirtualFilePtr>(file).release();
}

IVirtualFile * VirtualDirectoryPtr::GetFileRelative(const char * name) const
{
    if (m_directory.get() == nullptr)
    {
        return nullptr;
    }
    FileSys::VirtualFile file = m_directory->GetFileRelative(name);
    if (file.get() == nullptr)
    {
        return nullptr;
    }
    return std::make_unique<VirtualFilePtr>(file).release();
}

IVirtualFile * VirtualDirectoryPtr::OpenFile(const char * path, VirtualFileOpenMode perms)
{
    if (m_directory.get() == nullptr)
    {
        return nullptr;
    }
    UNIMPLEMENTED();
    return nullptr;
}

void VirtualDirectoryPtr::Release()
{
    delete this;
}

VirtualFilePtr::VirtualFilePtr(FileSys::VirtualFile & file) :
    m_file(file)
{
}

VirtualFilePtr::~VirtualFilePtr()
{
}

uint64_t VirtualFilePtr::GetSize() const
{
    if (m_file)
    {
        return m_file->GetSize();
    }
    return 0;
}

bool VirtualFilePtr::Resize(uint64_t size)
{
    if (m_file)
    {
        return m_file->Resize(size);
    }
    return 0;
}

uint64_t VirtualFilePtr::ReadBytes(uint8_t * data, uint64_t datalen, uint64_t offset)
{
    if (m_file)
    {
        return m_file->Read(data, datalen, offset);
    }
    return 0;
}

uint64_t VirtualFilePtr::WriteBytes(const uint8_t * data, uint64_t datalen, uint64_t offset)
{
    if (m_file)
    {
        return m_file->Write(data, datalen, offset);
    }
    return 0;
}

IVirtualDirectory * VirtualFilePtr::ExtractRomFS()
{
    FileSys::VirtualDir dir = FileSys::ExtractRomFS(m_file);
    if (dir.get() == nullptr)
    {
        return nullptr;
    }
    return std::make_unique<VirtualDirectoryPtr>(dir).release();
}

IVirtualFile * VirtualFilePtr::Duplicate()
{
    return std::make_unique<VirtualFilePtr>(m_file).release();
}

void VirtualFilePtr::Release()
{
    delete this;
}
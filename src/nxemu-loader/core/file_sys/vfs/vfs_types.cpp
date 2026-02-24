#include "vfs_types.h"
#include "core/file_sys/romfs.h"
#include <yuzu_common/yuzu_assert.h>

VirtualDirectoryImpl::VirtualDirectoryImpl(FileSys::VirtualDir & directory) :
    m_directory(directory)
{
}

VirtualDirectoryImpl::~VirtualDirectoryImpl()
{
}

IVirtualDirectory * VirtualDirectoryImpl::CreateSubdirectory(const char * path) const
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
    return std::make_unique<VirtualDirectoryImpl>(dir).release();
}

IVirtualDirectory * VirtualDirectoryImpl::GetDirectoryRelative(const char * path) const
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
    return std::make_unique<VirtualDirectoryImpl>(dir).release();
}

IVirtualDirectory * VirtualDirectoryImpl::GetSubdirectory(const char * path) const
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
    return std::make_unique<VirtualDirectoryImpl>(dir).release();
}

IVirtualDirectory * VirtualDirectoryImpl::Duplicate()
{
    return std::make_unique<VirtualDirectoryImpl>(m_directory).release();
}

IVirtualFile * VirtualDirectoryImpl::CreateFile(const char * name) const
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
    return std::make_unique<VirtualFileImpl>(file).release();
}

IVirtualFile * VirtualDirectoryImpl::GetFile(const char * name) const
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
    return std::make_unique<VirtualFileImpl>(file).release();
}

IVirtualFile * VirtualDirectoryImpl::GetFileRelative(const char * name) const
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
    return std::make_unique<VirtualFileImpl>(file).release();
}

IVirtualFile * VirtualDirectoryImpl::OpenFile(const char * path, VirtualFileOpenMode perms)
{
    if (m_directory.get() == nullptr)
    {
        return nullptr;
    }
    UNIMPLEMENTED();
    return nullptr;
}

void VirtualDirectoryImpl::Release()
{
    delete this;
}

VirtualFileImpl::VirtualFileImpl(FileSys::VirtualFile & file) :
    m_file(file)
{
}

VirtualFileImpl::~VirtualFileImpl()
{
}

uint64_t VirtualFileImpl::GetSize() const
{
    if (m_file)
    {
        return m_file->GetSize();
    }
    return 0;
}

bool VirtualFileImpl::Resize(uint64_t size)
{
    if (m_file)
    {
        return m_file->Resize(size);
    }
    return 0;
}

uint64_t VirtualFileImpl::ReadBytes(uint8_t * data, uint64_t datalen, uint64_t offset)
{
    if (m_file)
    {
        return m_file->Read(data, datalen, offset);
    }
    return 0;
}

uint64_t VirtualFileImpl::WriteBytes(const uint8_t * data, uint64_t datalen, uint64_t offset)
{
    if (m_file)
    {
        return m_file->Write(data, datalen, offset);
    }
    return 0;
}

IVirtualDirectory * VirtualFileImpl::ExtractRomFS()
{
    FileSys::VirtualDir dir = FileSys::ExtractRomFS(m_file);
    if (dir.get() == nullptr)
    {
        return nullptr;
    }
    return std::make_unique<VirtualDirectoryImpl>(dir).release();
}

IVirtualFile * VirtualFileImpl::Duplicate()
{
    return std::make_unique<VirtualFileImpl>(m_file).release();
}

void VirtualFileImpl::Release()
{
    delete this;
}
#include "vfs_ivirtualfile.h"

VfsVirtualFile::VfsVirtualFile(IVirtualFile * file) :
    m_file(file)
{
}

VfsVirtualFile::~VfsVirtualFile()
{
    if (m_file != nullptr)
    {
        m_file->Release();
    }
}

std::string VfsVirtualFile::GetName() const
{
    __debugbreak();
    return "";
}

std::string VfsVirtualFile::GetExtension() const
{
    __debugbreak();
    return "";
}

std::size_t VfsVirtualFile::GetSize() const
{
    return m_file->GetSize();
}

bool VfsVirtualFile::Resize(std::size_t new_size)
{
    __debugbreak();
    return false;
}

FileSys::VirtualDir VfsVirtualFile::GetContainingDirectory() const
{
    //__debugbreak();
    return {};
}

bool VfsVirtualFile::IsWritable() const
{
    __debugbreak();
    return false;
}

bool VfsVirtualFile::IsReadable() const
{
    __debugbreak();
    return false;
}

std::size_t VfsVirtualFile::Read(u8 * data, std::size_t length, std::size_t offset) const
{
    return m_file->ReadBytes(data, length, offset);
}

std::size_t VfsVirtualFile::Write(const u8 * data, std::size_t length, std::size_t offset)
{
    __debugbreak();
    return 0;
}

bool VfsVirtualFile::Rename(std::string_view name)
{
    __debugbreak();
    return false;
}

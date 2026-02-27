#include "filesystem_interfaces.h"
#include <nxemu-module-spec/system_loader.h>
#include <yuzu_common/interface_pointer_def.h>

std::vector<uint8_t> IVirtualFilePtr::ReadAllBytes() const
{
    uint64_t dataSize = m_ptr->GetSize();
    if (dataSize == 0)
    {
        return {};
    }
    std::vector<uint8_t> data;
    data.resize(dataSize);
    dataSize = m_ptr->ReadBytes(data.data(), data.size(), 0);
    if (dataSize < data.size())
    {
        data.resize(dataSize);
    }
    return data;
}

template class InterfacePtr<IVirtualDirectoryList>;
template class InterfacePtr<IVirtualDirectory>;
template class InterfacePtr<IVirtualFileList>;
template class InterfacePtr<IVirtualFile>;
template class InterfacePtr<IFileSysNCA>;
template class InterfacePtr<ISaveDataFactory>;
template class InterfacePtr<IRomFsController>;
template class InterfacePtr<ISaveDataController>;
template class InterfacePtr<IFileSysNACP>;

#pragma once
#include <vector>
#include "yuzu_common/interface_pointer.h"

__interface IVirtualDirectoryList;
__interface IVirtualDirectory;
__interface IVirtualFileList;
__interface IVirtualFile;
__interface IFileSysNCA;
__interface ISaveDataFactory;
__interface IRomFsController;
__interface ISaveDataController;
__interface IFileSysNACP;

using IVirtualDirectoryListPtr = InterfacePtr<IVirtualDirectoryList>;
using IVirtualDirectoryPtr = InterfacePtr<IVirtualDirectory>;
using FileSysNCAPtr = InterfacePtr<IFileSysNCA>;
using SaveDataFactoryPtr = InterfacePtr<ISaveDataFactory>;
using RomFsControllerPtr = InterfacePtr<IRomFsController>;
using ISaveDataControllerPtr = InterfacePtr<ISaveDataController>;
using IFileSysNACPPtr = InterfacePtr<IFileSysNACP>;
using IVirtualFileListPtr = InterfacePtr<IVirtualFileList>;

class IVirtualFilePtr : public InterfacePtr<IVirtualFile>
{
public:
    std::vector<uint8_t> ReadAllBytes() const;
};

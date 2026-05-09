#pragma once
#include "yuzu_common/interface_pointer.h"
#include <nxemu-module-spec/base.h>
#include <vector>

nxinterface IVirtualDirectoryList;
nxinterface IVirtualDirectory;
nxinterface IVirtualFileList;
nxinterface IVirtualFile;
nxinterface IFileSysNCA;
nxinterface ISaveDataFactory;
nxinterface IRomFsController;
nxinterface ISaveDataController;
nxinterface IFileSysNACP;

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

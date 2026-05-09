// SPDX-FileCopyrightText: Copyright 2024 yuzu Emulator Project
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "core/file_sys/errors.h"
#include "core/file_sys/fs_directory.h"
#include "core/file_sys/fs_file.h"
#include "core/file_sys/fs_filesystem.h"
#include "core/hle/result.h"
#include <nxemu-module-spec/system_loader.h>
#include <yuzu_common/common_types.h>
#include <yuzu_common/fs/filesystem_interfaces.h>

namespace FileSys::Fsa
{

class IDirectory
{
public:
    explicit IDirectory(IVirtualDirectoryPtr && backend_, OpenDirectoryMode mode) :
        backend(std::move(backend_))
    {
        // TODO(DarkLordZach): Verify that this is the correct behavior.
        // Build entry index now to save time later.
        if (True(mode & OpenDirectoryMode::Directory))
        {
            IVirtualDirectoryListPtr subdirectories(backend->GetSubdirectories());
            BuildEntryIndex(subdirectories);
        }
        if (True(mode & OpenDirectoryMode::File))
        {
            IVirtualFileListPtr files(backend->GetFiles());
            BuildEntryIndex(files);
        }
    }
    virtual ~IDirectory()
    {
    }

    Result Read(s64 * out_count, DirectoryEntry * out_entries, s64 max_entries)
    {
        R_UNLESS(out_count != nullptr, ResultNullptrArgument);
        if (max_entries == 0)
        {
            *out_count = 0;
            R_SUCCEED();
        }
        R_UNLESS(out_entries != nullptr, ResultNullptrArgument);
        R_UNLESS(max_entries > 0, ResultInvalidArgument);
        R_RETURN(this->DoRead(out_count, out_entries, max_entries));
    }

    Result GetEntryCount(s64 * out)
    {
        R_UNLESS(out != nullptr, ResultNullptrArgument);
        R_RETURN(this->DoGetEntryCount(out));
    }

private:
    Result DoRead(s64 * out_count, DirectoryEntry * out_entries, s64 max_entries)
    {
        const u64 actual_entries = std::min(static_cast<u64>(max_entries), entries.size() - next_entry_index);
        const auto * begin = reinterpret_cast<u8 *>(entries.data() + next_entry_index);
        const auto * end = reinterpret_cast<u8 *>(entries.data() + next_entry_index + actual_entries);
        const auto range_size = static_cast<std::size_t>(std::distance(begin, end));

        next_entry_index += actual_entries;
        *out_count = actual_entries;

        std::memcpy(out_entries, begin, range_size);

        R_SUCCEED();
    }

    Result DoGetEntryCount(s64 * out)
    {
        *out = entries.size() - next_entry_index;
        R_SUCCEED();
    }

    void BuildEntryIndex(IVirtualDirectoryListPtr & new_data)
    {
        uint32_t new_data_size = new_data->GetSize();
        if (new_data_size == 0)
        {
            return;
        }
        entries.reserve(entries.size() + new_data_size);

        for (uint32_t i = 0; i < new_data_size; i++)
        {
            IVirtualDirectoryPtr new_entry(new_data->GetItem(i));
            entries.emplace_back(new_entry->GetName(), static_cast<s8>(DirectoryEntryType::Directory), 0);
        }
    }

    void BuildEntryIndex(IVirtualFileListPtr & new_data)
    {
        uint32_t new_data_size = new_data->GetSize();
        if (new_data_size == 0)
        {
            return;
        }
        entries.reserve(entries.size() + new_data_size);

        for (uint32_t i = 0; i < new_data_size; i++)
        {
            IVirtualFilePtr new_entry(new_data->GetItem(i));
            entries.emplace_back(new_entry->GetName(), static_cast<s8>(DirectoryEntryType::File), new_entry->GetSize());
        }
    }

    IVirtualDirectoryPtr backend;
    std::vector<DirectoryEntry> entries;
    u64 next_entry_index = 0;
};

} // namespace FileSys::Fsa

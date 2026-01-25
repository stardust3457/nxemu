#include "rom_info.h"
#include "core/file_sys/registered_cache.h"
#include "core/file_sys/card_image.h"
#include "core/file_sys/submission_package.h"
#include "core/file_sys/content_archive.h"

#include "core/loader/loader.h"

RomInfo::RomInfo(FileSys::VirtualFile file, std::shared_ptr<Loader::AppLoader> loader) :
    m_file(file),
    m_loader(std::move(loader))
{
}

RomInfo::~RomInfo()
{
}

LoaderFileType RomInfo::GetFileType() const
{
    if (m_loader)
    {
        return m_loader->GetFileType();
    }
    return LoaderFileType::Error;
}

LoaderResultStatus RomInfo::ReadProgramId(uint64_t & out_program_id)
{
    if (m_loader)
    {
        return m_loader->ReadProgramId(out_program_id);
    }
    return LoaderResultStatus::ErrorNotInitialized;
}

LoaderResultStatus RomInfo::ReadTitle(char * buffer, uint32_t * bufferSize)
{
    if (m_loader)
    {
        return m_loader->ReadTitle(buffer, bufferSize);
    }
    return LoaderResultStatus::ErrorNotInitialized;
}

LoaderResultStatus RomInfo::ReadIcon(uint8_t * buffer, uint32_t * bufferSize)
{
    if (m_loader)
    {
        return m_loader->ReadIcon(buffer, bufferSize);
    }
    return LoaderResultStatus::ErrorNotInitialized;
}

LoaderResultStatus RomInfo::ReadProgramIds(uint64_t * buffer, uint32_t * count)
{
    if (m_loader)
    {
        return m_loader->ReadProgramIds(buffer, count);
    }
    return LoaderResultStatus::ErrorNotInitialized;
}

void RomInfo::AddToManualContentProvider(IManualContentProvider & provider)
{
    const LoaderFileType file_type = GetFileType();
    if (file_type == LoaderFileType::NCA)
    {
        u64 program_id = 0;
        ReadProgramId(program_id);
        provider.AddEntry(LoaderTitleType::Application, FileSys::GetCRTypeFromNCAType(FileSys::NCA{m_file}.GetType()), program_id, std::make_unique<VirtualFilePtr>(m_file).release());
    }
    else if (file_type == LoaderFileType::XCI || file_type == LoaderFileType::NSP)
    {
        const std::shared_ptr<FileSys::NSP> nsp = file_type == LoaderFileType::NSP ? std::make_shared<FileSys::NSP>(m_file) : FileSys::XCI{m_file}.GetSecurePartitionNSP();
        for (const auto & title : nsp->GetNCAs())
        {
            for (const auto & entry : title.second)
            {
                FileSys::VirtualFile file = entry.second->GetBaseFile();
                provider.AddEntry(entry.first.first, entry.first.second, title.first, std::make_unique<VirtualFilePtr>(file).release());
            }
        }
    }
}

void RomInfo::Release()
{
    delete this;
}
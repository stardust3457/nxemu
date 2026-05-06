// SPDX-FileCopyrightText: Copyright 2024 yuzu Emulator Project
// SPDX-License-Identifier: GPL-2.0-or-later

#include "yuzu_common/scope_exit.h"

#include "core/core.h"
#include "core/file_sys/filesystem_interfaces.h"
#include "core/hle/kernel/k_process.h"
#include "core/hle/service/am/process.h"
#include <nxemu-module-spec/system_loader.h>

namespace Service::AM
{

Process::Process(Core::System & system) :
    m_system(system),
    m_process(),
    m_main_thread_priority(),
    m_main_thread_stack_size(),
    m_program_id(),
    m_process_started()
{
}

Process::~Process()
{
    this->Finalize();
}

bool Process::Initialize(u64 program_id)
{
    this->Finalize();

    ISystemloader & loader = m_system.GetSystemloader();
    IFileSystemController & fsc = loader.FileSystemController();

    IFileSysRegisteredCache & nand = fsc.GetSystemNANDContents();
    nand.GetEntry(program_id, LoaderContentRecordType::Program);
    FileSysNCAPtr nca(nand.GetEntry(program_id, LoaderContentRecordType::Program));

    if (!nca)
    {
        return false;
    }

    UNIMPLEMENTED();
    return true;
}

void Process::Finalize()
{
    // Terminate, if we are currently holding a process.
    this->Terminate();

    // Close the process.
    if (m_process)
    {
        m_process->Close();

        // TODO: remove this, kernel already tracks this
        m_system.Kernel().RemoveProcess(m_process);
    }

    // Clean up.
    m_process = nullptr;
    m_main_thread_priority = 0;
    m_main_thread_stack_size = 0;
    m_program_id = 0;
    m_process_started = false;
}

bool Process::Run()
{
    // If we already started the process, don't start again.
    if (m_process_started)
    {
        return false;
    }

    // Start.
    if (m_process)
    {
        m_process->Run(m_main_thread_priority, m_main_thread_stack_size);
    }

    // Mark as started.
    m_process_started = true;

    // We succeeded.
    return true;
}

void Process::Terminate()
{
    if (m_process)
    {
        m_process->Terminate();
    }
}

u64 Process::GetProcessId() const
{
    if (m_process)
    {
        return m_process->GetProcessId();
    }

    return 0;
}

} // namespace Service::AM

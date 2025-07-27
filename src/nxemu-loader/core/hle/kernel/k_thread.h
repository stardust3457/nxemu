// SPDX-FileCopyrightText: Copyright 2021 yuzu Emulator Project
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <array>
#include <atomic>
#include <condition_variable>
#include <mutex>
#include <span>
#include <string>
#include <utility>
#include <vector>

#include "yuzu_common/intrusive_list.h"

#include "yuzu_common/intrusive_red_black_tree.h"
#include "yuzu_common/scratch_buffer.h"
#include "yuzu_common/spin_lock.h"

namespace Common {
class Fiber;
}

namespace Core {
namespace Memory {
class Memory;
}
class System;
} // namespace Core

namespace Kernel {

class GlobalSchedulerContext;
class KernelCore;
class KProcess;
class KScheduler;
class KThreadQueue;

using KThreadFunction = KProcessAddress;

enum class ThreadType : u32 {
    Main = 0,
    Kernel = 1,
    HighPriority = 2,
    User = 3,
    Dummy = 100, // Special thread type for emulation purposes only
};
DECLARE_ENUM_FLAG_OPERATORS(ThreadType);

enum class SuspendType : u32 {
    Process = 0,
    Thread = 1,
    Debug = 2,
    Backtrace = 3,
    Init = 4,
    System = 5,

    Count,
};

enum class ThreadState : u16 {
    Initialized = 0,
    Waiting = 1,
    Runnable = 2,
    Terminated = 3,

    SuspendShift = 4,
    Mask = (1 << SuspendShift) - 1,

    ProcessSuspended = (1 << (0 + SuspendShift)),
    ThreadSuspended = (1 << (1 + SuspendShift)),
    DebugSuspended = (1 << (2 + SuspendShift)),
    BacktraceSuspended = (1 << (3 + SuspendShift)),
    InitSuspended = (1 << (4 + SuspendShift)),
    SystemSuspended = (1 << (5 + SuspendShift)),

    SuspendFlagMask = ((1 << 6) - 1) << SuspendShift,
};
DECLARE_ENUM_FLAG_OPERATORS(ThreadState);

enum class DpcFlag : u32 {
    Terminating = (1 << 0),
    Terminated = (1 << 1),
};

enum class ThreadWaitReasonForDebugging : u32 {
    None,            ///< Thread is not waiting
    Sleep,           ///< Thread is waiting due to a SleepThread SVC
    IPC,             ///< Thread is waiting for the reply from an IPC request
    Synchronization, ///< Thread is waiting due to a WaitSynchronization SVC
    ConditionVar,    ///< Thread is waiting due to a WaitProcessWideKey SVC
    Arbitration,     ///< Thread is waiting due to a SignalToAddress/WaitForAddress SVC
    Suspended,       ///< Thread is waiting due to process suspension
};

enum class StepState : u32 {
    NotStepping,   ///< Thread is not currently stepping
    StepPending,   ///< Thread will step when next scheduled
    StepPerformed, ///< Thread has stepped, waiting to be scheduled again
};

namespace KThread
{
static constexpr s32 DefaultThreadPriority = 44;
}
} // namespace Kernel

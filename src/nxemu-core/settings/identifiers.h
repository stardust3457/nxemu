#pragma once
#include <stdint.h>

enum class EmulationState : int32_t
{
    Stopped,
    Starting,
    LoadingRom,
    Running,
    Paused,
    Stopping,
};

namespace NXCoreSetting
{
constexpr const char * GameFile = "nxcore:GameFile";
constexpr const char * GameName = "nxcore:GameName";
constexpr const char * ModuleDirectory = "nxcore:ModuleDirectory";
constexpr const char * ModuleLoader = "nxcore:ModuleLoader";
constexpr const char * ModuleCpu = "nxcore:ModuleCpu";
constexpr const char * ModuleVideo = "nxcore:ModuleVideo";
constexpr const char * ModuleOs = "nxcore:ModuleOs";
constexpr const char * ShowLogConsole = "nxcore:ShowLogConsole";
constexpr const char * LogFilter = "nxcore:LogFilter";
constexpr const char * EmulationRunning = "nxcore:EmulationRunning";
constexpr const char * EmulationState = "nxcore:EmulationState";
constexpr const char * DisplayedFrames = "nxcore:DisplayedFrames";
constexpr const char * ShuttingDown = "nxcore:ShuttingDown";

} // namespace NXCoreSetting
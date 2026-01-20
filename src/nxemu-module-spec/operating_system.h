#pragma once
#include "base.h"
#include <stdint.h>

enum class ProgramAddressSpaceType : uint8_t
{
    Is32Bit = 0,
    Is36Bit = 1,
    Is32BitNoMap = 2,
    Is39Bit = 3,
};

enum class PoolPartition : uint32_t
{
    Application = 0,
    Applet = 1,
    System = 2,
    SystemNonSecure = 3,
};

enum class StorageId : uint8_t 
{
    None = 0,
    Host = 1,
    GameCard = 2,
    NandSystem = 3,
    NandUser = 4,
    SdCard = 5,
};

// This is nn::hid::NpadIdType
enum class NpadIdType : uint32_t
{
    Player1 = 0x0,
    Player2 = 0x1,
    Player3 = 0x2,
    Player4 = 0x3,
    Player5 = 0x4,
    Player6 = 0x5,
    Player7 = 0x6,
    Player8 = 0x7,
    Other = 0x10,
    Handheld = 0x20,

    Invalid = 0xFFFFFFFF,
};

enum class NativeButtonValues : uint32_t
{
    A,
    B,
    X,
    Y,
    LStick,
    RStick,
    L,
    R,
    ZL,
    ZR,
    Plus,
    Minus,

    DLeft,
    DUp,
    DRight,
    DDown,

    SLLeft,
    SRLeft,

    Home,
    Screenshot,

    SLRight,
    SRRight,

    NumButtons,
};

enum class NativeMotionValues : uint32_t
{
    MotionLeft,
    MotionRight,

    NumMotions,
};

enum class NativeAnalogValues : uint32_t {
    LStick,
    RStick,

    NumAnalogs,
};

enum class ButtonNames : uint32_t
{
    Undefined,
    Invalid,
    // This will display the engine name instead of the button name
    Engine,
    // This will display the button by value instead of the button name
    Value,

    // Joycon button names
    ButtonLeft,
    ButtonRight,
    ButtonDown,
    ButtonUp,
    ButtonA,
    ButtonB,
    ButtonX,
    ButtonY,
    ButtonPlus,
    ButtonMinus,
    ButtonHome,
    ButtonCapture,
    ButtonStickL,
    ButtonStickR,
    TriggerL,
    TriggerZL,
    TriggerSL,
    TriggerR,
    TriggerZR,
    TriggerSR,

    // GC button names
    TriggerZ,
    ButtonStart,

    // DS4 button names
    L1,
    L2,
    L3,
    R1,
    R2,
    R3,
    Circle,
    Cross,
    Square,
    Triangle,
    Share,
    Options,
    Home,
    Touch,

    // Mouse buttons
    ButtonMouseWheel,
    ButtonBackward,
    ButtonForward,
    ButtonTask,
    ButtonExtra,
};

// This is nn::hid::NpadStyleIndex
enum class NpadStyleIndex : uint8_t {
    None = 0,
    Fullkey = 3,
    Handheld = 4,
    HandheldNES = 4,
    JoyconDual = 5,
    JoyconLeft = 6,
    JoyconRight = 7,
    GameCube = 8,
    Pokeball = 9,
    NES = 10,
    SNES = 12,
    N64 = 13,
    SegaGenesis = 14,
    SystemExt = 32,
    System = 33,
    MaxNpadType = 34,
};

// This is nn::hid::NpadStyleSet
enum class NpadStyleSet : uint32_t {
    None = 0,
    Fullkey = 1U << 0,
    Handheld = 1U << 1,
    JoyDual = 1U << 2,
    JoyLeft = 1U << 3,
    JoyRight = 1U << 4,
    Gc = 1U << 5,
    Palma = 1U << 6,
    Lark = 1U << 7,
    HandheldLark = 1U << 8,
    Lucia = 1U << 9,
    Lagoon = 1U << 10,
    Lager = 1U << 11,
    SystemExt = 1U << 29,
    System = 1U << 30,

    All = 0xFFFFFFFFU,
};

enum class PollingInputType 
{
    None, 
    Button, 
    Stick, 
    Motion, 
    Touch 
};

enum class ControllerTriggerType : uint32_t {
    Button,
    Stick,
    Trigger,
    Motion,
    Color,
    Battery,
    Vibration,
    IrSensor,
    RingController,
    Nfc,
    Connected,
    Disconnected,
    Type,
    All,
};

__interface IProgramMetadata
{
    bool Is64BitProgram() const = 0;
    ProgramAddressSpaceType GetAddressSpaceType() const = 0;
    uint8_t GetMainThreadPriority() const = 0;
    uint8_t GetMainThreadCore() const = 0;
    uint32_t GetMainThreadStackSize() const = 0;
    uint64_t GetTitleID() const = 0;
    uint64_t GetFilesystemPermissions() const = 0;
    uint32_t GetSystemResourceSize() const = 0;
    PoolPartition GetPoolPartition() const = 0;
    const uint32_t * GetKernelCapabilities() const = 0;
    uint32_t GetKernelCapabilitiesSize() const = 0;
    const char * GetName() const;
};

__interface IModuleInfo
{
    const uint8_t * Data(void) const = 0;
    uint32_t DataSize(void) const = 0;
    uint64_t CodeSegmentAddr(void) const = 0;
    uint64_t CodeSegmentOffset(void) const = 0;
    uint64_t CodeSegmentSize(void) const = 0;
    uint64_t RODataSegmentAddr(void) const = 0;
    uint64_t RODataSegmentOffset(void) const = 0;
    uint64_t RODataSegmentSize(void) const = 0;
    uint64_t DataSegmentAddr(void) const = 0;
    uint64_t DataSegmentOffset(void) const = 0;
    uint64_t DataSegmentSize(void) const = 0;
};

__interface IDeviceMemory
{
    const uint8_t * BackingBasePointer() const = 0;
};

__interface ICacheInvalidator 
{
    virtual void OnCacheInvalidation(uint64_t address, uint32_t size) = 0;
};

typedef void (*DeviceEnumCallback)(const char * device, void * userData);

__interface IParamPackage
{
    bool Has(const char * key) const = 0;
    bool GetBool(const char * key, bool default_value) const = 0;
    int32_t GetInt(const char * key, int32_t default_value) const = 0;
    float GetFloat(const char * key, float default_value) const = 0;
    const char * GetString(const char * key, const char * default_value) const = 0;
    void SetFloat(const char * key, float value) = 0;
    const char * Serialize() const = 0;

    void Release() = 0;
};

__interface IParamPackageList
{
    uint32_t GetCount() const = 0;
    IParamPackage & GetParamPackage(uint32_t index) const = 0;
    void Release() = 0;
};

struct vec3f_t
{
    float x;
    float y;
    float z;
};

struct ControllerMotion
{
    vec3f_t accel;
    vec3f_t gyro;
    vec3f_t rotation;
    vec3f_t euler;
    vec3f_t orientation[3];
    bool atRest;
};

struct MotionState
{
    ControllerMotion motion[2];
};

// Analog properties for calibration
struct AnalogProperties 
{
    // Anything below this value will be detected as zero
    float deadzone;
    // Anything above this values will be detected as one
    float range;
    // Minimum value to be detected as active
    float threshold;
    // Drift correction applied to the raw data
    float offset;
    // Invert direction of the sensor data
    bool inverted;
    // Invert the state if it's converted to a button
    bool inverted_button;
    // Press once to activate, press again to release
    bool toggle;
};

// Single analog sensor data
struct AnalogStatus {
    float value;
    float raw_value;
    AnalogProperties properties;
};

// Analog and digital joystick data
struct StickStatus {
    unsigned char uuid[16];
    AnalogStatus x{};
    AnalogStatus y{};
    bool left;
    bool right;
    bool up;
    bool down;
};

struct SticksValues
{
    StickStatus status[(size_t)NativeAnalogValues::NumAnalogs];
};

typedef struct {
    unsigned char uuid[16];
    bool value;
    bool inverted;            // Invert value of the button
    bool toggle;              // Press once to activate, press again to release
    bool turbo;               // Spams the button when active
    bool locked;              // Internal lock for the toggle status
} button_status_t;

typedef void (CALL* ControllerEventCallback)(ControllerTriggerType type, void * user);

__interface IEmulatedController
{
    void Connect(bool use_temporary_value = false) = 0;
    void Disconnect() = 0;
    bool IsConnected(bool get_temporary_value = false) const = 0;
    void SaveCurrentConfig() = 0;
    void ReloadFromSettings() = 0;
    IParamPackageList * GetMappedDevicesPtr() const = 0;
    IParamPackage * GetButtonParamPtr(uint32_t index) const = 0;
    IParamPackage * GetMotionParamPtr(uint32_t index) const = 0;
    IParamPackage * GetStickParamPtr(uint32_t index) const = 0;
    NpadStyleIndex GetNpadStyleIndex(bool getTemporaryValue = false) const = 0;
    void SetButtonParam(uint32_t index, const IParamPackage & param) = 0;
    void SetStickParam(uint32_t index, const IParamPackage & param) = 0;
    void SetMotionParam(uint32_t index, const IParamPackage & param) = 0;
    void SetControllerEventCallback(ControllerEventCallback cb, void * user) = 0;
    void GetButtonsStatus(button_status_t * buttons, size_t num_buttons) const = 0;
    void SetNpadStyleIndex(NpadStyleIndex npad_type) = 0;
    MotionState GetMotions() const = 0;
    SticksValues GetSticksValues() const = 0;
};

__interface IButtonMappingList
{
    uint32_t GetCount() const = 0;
    uint32_t GetIndex(uint32_t position) const = 0;
    IParamPackage& GetParamPackage(uint32_t position) const = 0;
    void Release() = 0;
};

__interface IOperatingSystem
{
    bool Initialize() = 0;
    void ShutDown() = 0;
    bool IsShuttingDown() const = 0;
    void ShutdownMainProcess() = 0;
    bool CreateApplicationProcess(uint64_t codeSize, const IProgramMetadata & metaData, uint64_t & baseAddress, uint64_t & processID, bool is_hbl) = 0;
    void StartApplicationProcess(int32_t priority, int64_t stackSize, uint32_t version, StorageId baseGameStorageId, StorageId updateStorageId, uint8_t * nacpData, uint32_t nacpDataLen) = 0;
    bool LoadModule(const IModuleInfo & module, uint64_t baseAddress) = 0;
    IDeviceMemory & DeviceMemory() = 0;
    void KeyboardKeyPress(int modifier, int keyIndex, int keyCode) = 0;
    void KeyboardKeyRelease(int modifier, int keyIndex, int keyCode) = 0;
    void GatherGPUDirtyMemory(ICacheInvalidator * invalidator) = 0;
    uint64_t GetGPUTicks() = 0;
    uint64_t GetProgramId() = 0;
    bool GetExitLocked() const = 0;
    void GameFrameEnd() = 0;
    void AudioGetSyncIDs(uint32_t * ids, uint32_t maxCount, uint32_t * actualCount) = 0;
    void AudioGetDeviceListForSink(uint32_t sinkId, bool capture, DeviceEnumCallback callback, void * userData) = 0;
    void RegisterHostThread() = 0;
    IParamPackageList * GetInputDevices() const = 0;
    IEmulatedController & GetEmulatedController(NpadIdType index) = 0;
    ButtonNames GetButtonName(const IParamPackage & param)  const = 0;
    bool IsController(const IParamPackage & params)  const = 0;
    NpadStyleSet GetSupportedStyleTag() const = 0;
    IButtonMappingList * GetButtonMappingForDevice(const IParamPackage & param) const = 0;
    IButtonMappingList * GetAnalogMappingForDevice(const IParamPackage & param) const = 0;
    IButtonMappingList * GetMotionMappingForDevice(const IParamPackage & param) const = 0;
    void BeginMapping(PollingInputType type) = 0;
    void StopMapping() = 0;
    IParamPackage * GetNextInput() const = 0;
    void PumpInputEvents() const = 0;
};

EXPORT IOperatingSystem * CALL CreateOperatingSystem(ISystemModules & modules);
EXPORT void CALL DestroyOperatingSystem(IOperatingSystem * operatingSystem);

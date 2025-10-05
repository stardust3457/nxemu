// SPDX-FileCopyrightText: Copyright 2018 yuzu Emulator Project
// SPDX-License-Identifier: GPL-2.0-or-later

#include "core/core_timing.h"
#include "yuzu_hid_core/frontend/emulated_devices.h"
#include "yuzu_hid_core/hid_core.h"
#include "yuzu_hid_core/resources/applet_resource.h"
#include "yuzu_hid_core/resources/mouse/mouse.h"
#include "yuzu_hid_core/resources/shared_memory_format.h"

namespace Service::HID {

Mouse::Mouse(Core::HID::HIDCore& hid_core_) : ControllerBase{hid_core_} {
    emulated_devices = hid_core.GetEmulatedDevices();
}

Mouse::~Mouse() = default;

void Mouse::OnInit() {}
void Mouse::OnRelease() {}

void Mouse::OnUpdate(const Core::Timing::CoreTiming& core_timing) {
    std::scoped_lock shared_lock{*shared_mutex};
    const u64 aruid = applet_resource->GetActiveAruid();
    auto* data = applet_resource->GetAruidData(aruid);

    if (data == nullptr || !data->flag.is_assigned) {
        return;
    }

    MouseSharedMemoryFormat& shared_memory = data->shared_memory_format->mouse;

    if (!IsControllerActivated()) {
        shared_memory.mouse_lifo.buffer_count = 0;
        shared_memory.mouse_lifo.buffer_tail = 0;
        return;
    }

    next_state = {};

    const auto& last_entry = shared_memory.mouse_lifo.ReadCurrentEntry().state;
    next_state.sampling_number = last_entry.sampling_number + 1;

    if (Settings::values.mouse_enabled) {
        UNIMPLEMENTED();
    }

    shared_memory.mouse_lifo.WriteNextEntry(next_state);
}

} // namespace Service::HID

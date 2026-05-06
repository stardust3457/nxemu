// SPDX-FileCopyrightText: Copyright 2019 yuzu Emulator Project
// SPDX-License-Identifier: GPL-2.0-or-later

#include "applets/general.h"
#include "yuzu_common/logging/log.h"

void DefaultParentalControlsApplet::Close()
{
}

void DefaultParentalControlsApplet::VerifyPIN(void * user_data, BoolFinishedFn finished, bool suspend_future_verification_temporarily)
{
    LOG_INFO(Service_AM, "Application requested frontend to verify PIN (normal), suspend_future_verification_temporarily={}, verifying as correct.", suspend_future_verification_temporarily);
    finished(user_data, true);
}

void DefaultParentalControlsApplet::VerifyPINForSettings(void * user_data, BoolFinishedFn finished)
{
    LOG_INFO(Service_AM, "Application requested frontend to verify PIN (settings), verifying as correct.");
    finished(user_data, true);
}

void DefaultParentalControlsApplet::RegisterPIN(void * user_data, SimpleFinishedFn finished)
{
    LOG_INFO(Service_AM, "Application requested frontend to register new PIN");
    finished(user_data);
}

void DefaultParentalControlsApplet::ChangePIN(void * user_data, SimpleFinishedFn finished)
{
    LOG_INFO(Service_AM, "Application requested frontend to change PIN to new value");
    finished(user_data);
}

void DefaultPhotoViewerApplet::Close()
{
}

void DefaultPhotoViewerApplet::ShowPhotosForApplication(uint64_t title_id, void * user_data, SimpleFinishedFn finished) const
{
    LOG_INFO(Service_AM, "Application requested frontend to display stored photos for title_id={:016X}", title_id);
    finished(user_data);
}

void DefaultPhotoViewerApplet::ShowAllPhotos(void * user_data, SimpleFinishedFn finished) const
{
    LOG_INFO(Service_AM, "Application requested frontend to display all stored photos.");
    finished(user_data);
}
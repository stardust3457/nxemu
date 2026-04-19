// SPDX-FileCopyrightText: Copyright 2022 yuzu Emulator Project
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <functional>

#include "applets/applet.h"

class MiiEditApplet : public Applet
{
public:
    using MiiEditCallback = std::function<void()>;

    virtual ~MiiEditApplet();

    virtual void ShowMiiEdit(const MiiEditCallback & callback) const = 0;
};

class DefaultMiiEditApplet final : public MiiEditApplet
{
public:
    void Close() const override;
    void ShowMiiEdit(const MiiEditCallback & callback) const override;
};
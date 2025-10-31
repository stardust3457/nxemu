#pragma once
#include <sciter_element.h>
#include <nxemu-module-spec/operating_system.h>

class InputConfig;

class InputConfigPlayer
{
public:
    InputConfigPlayer(ISciterUI& sciterUI, InputConfig & config, HWINDOW parent, SciterElement page, NpadIdType controllerIndex);
    ~InputConfigPlayer() = default;

private:
    InputConfigPlayer() = delete;
    InputConfigPlayer(const InputConfigPlayer&) = delete;
    InputConfigPlayer& operator=(const InputConfigPlayer&) = delete;

    ISciterUI & m_sciterUI;
    InputConfig & m_config;
    HWINDOW m_parent;
    SciterElement m_page;
    NpadIdType m_controllerIndex;
};

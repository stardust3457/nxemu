#include "input_config_player.h"

InputConfigPlayer::InputConfigPlayer(ISciterUI & sciterUI, InputConfig & config, HWINDOW parent, SciterElement page, NpadIdType controllerIndex) :
    m_sciterUI(sciterUI),
    m_config(config),
    m_parent(parent),
    m_page(page),
    m_controllerIndex(controllerIndex)
{
}

#include "input_config.h"
#include <sciter_ui.h>

InputConfig::InputConfig(ISciterUI & SciterUI) :
    m_sciterUI(SciterUI),
    m_window(nullptr)
{
}

InputConfig::~InputConfig()
{
}

void InputConfig::Display(void * parentWindow)
{
    if (!m_sciterUI.WindowCreate(parentWindow, "input_config.html", 0, 0, 300, 300, SUIW_CHILD, m_window))
    {
        return;
    }
    m_window->FixMinSize();
    m_window->CenterWindow();
}

#pragma once
#include <sciter_handler.h>
#include <widgets/page_nav.h>
#include <memory>

__interface ISciterUI;
__interface ISciterWindow;
class InputConfigPlayer;

class InputConfig :
    public IPagesSink,
    public IClickSink
{
public:
    InputConfig(ISciterUI & SciterUI);
    ~InputConfig();

    void Display(void * parentWindow);

    //__interface IPagesSink
    bool PageNavChangeFrom(const std::string & pageName, SCITER_ELEMENT pageNav) override;
    bool PageNavChangeTo(const std::string & pageName, SCITER_ELEMENT pageNav) override;
    void PageNavCreatedPage(const std::string & pageName, SCITER_ELEMENT page) override;
    void PageNavPageChanged(const std::string & pageName, SCITER_ELEMENT pageNav) override;

    // IClickSink
    bool OnClick(SCITER_ELEMENT element, SCITER_ELEMENT source, uint32_t reason) override;

private:
    InputConfig() = delete;
    InputConfig(const InputConfig &) = delete;
    InputConfig & operator=(const InputConfig &) = delete;

    ISciterUI & m_sciterUI;
    ISciterWindow * m_window;
    std::shared_ptr<IPageNav> m_pageNav;
    std::unique_ptr<InputConfigPlayer> m_playerConfig[8];
};

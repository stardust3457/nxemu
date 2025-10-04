#include "config_setting.h"
#include "settings/ui_settings.h"
#include "system_config.h"
#include "system_config_game_browser.h"
#include <common/path.h>
#include <nxemu-core/settings/identifiers.h>

SystemConfigGameBrowser::SystemConfigGameBrowser(ISciterUI & sciterUI, SystemConfig & config, ISciterWindow & window, SciterElement page) :
    m_sciterUI(sciterUI),
    m_config(config),
    m_window(window),
    m_page(page)
{
    SciterElement pageNav = page.GetElementByID("GameBrowserTabNav");
    std::shared_ptr<void> interfacePtr = pageNav.IsValid() ? m_sciterUI.GetElementInterface(pageNav, IID_IPAGENAV) : nullptr;
    if (interfacePtr)
    {
        m_pageNav = std::static_pointer_cast<IPageNav>(interfacePtr);
        m_pageNav->AddSink(this);
    }
}

bool SystemConfigGameBrowser::PageNavChangeFrom(const std::string& /*pageName*/, SCITER_ELEMENT /*pageNav*/)
{
    return true;
}

bool SystemConfigGameBrowser::PageNavChangeTo(const std::string& /*pageName*/, SCITER_ELEMENT /*pageNav*/)
{
    return true;
}

void SystemConfigGameBrowser::PageNavCreatedPage(const std::string & pageName, SCITER_ELEMENT page)
{
    if (pageName == "GameBrowser")
    {
        SetupGameBrowserPage(page);
    }
}

void SystemConfigGameBrowser::PageNavPageChanged(const std::string& /*pageName*/, SCITER_ELEMENT /*pageNav*/)
{
}

bool SystemConfigGameBrowser::OnClick(SCITER_ELEMENT element, SCITER_ELEMENT source, uint32_t /*reason*/)
{
    SciterElement clickElem(element);
    std::string elementID = clickElem.GetAttributeByName("id");
    if (elementID == "gameDirectoryList")
    {
        SelectDirectoryItem(clickElem, source);
    }
    else if (elementID == "gameDirectoryAdd")
    {
        Path newDirectory = Path().BrowseForDirectory((void*)m_window.GetHandle(), "Select Game Directory");
        if (newDirectory.DirectoryExists())
        {
            AddGameDirectory(newDirectory);
        }
    }
    else if (elementID == "gameDirectoryRemove")
    {
        RemoveSelectedDirectory();
    }
    return true;
}

void SystemConfigGameBrowser::SetupGameBrowserPage(SciterElement page)
{
    m_gameBrowserPage = page;

    SciterElement root(m_window.GetRootElement());
    if (root.IsValid())
    {        
        SciterElement gameDirectoryList = root.GetElementByID("gameDirectoryList");
        m_sciterUI.AttachHandler(gameDirectoryList, IID_ICLICKSINK, (IClickSink*)this);
        SciterElement addButton = root.GetElementByID("gameDirectoryAdd");
        m_sciterUI.AttachHandler(addButton, IID_ICLICKSINK, (IClickSink*)this);
        SciterElement removeButton = root.GetElementByID("gameDirectoryRemove");
        m_sciterUI.AttachHandler(removeButton, IID_ICLICKSINK, (IClickSink*)this);
    }

    for (size_t i = 0, n = uiSettings.gameDirectories.size(); i < n; i++)
    {
        AddGameDirectory(uiSettings.gameDirectories[i]);
    }
}

void SystemConfigGameBrowser::SelectDirectoryItem(const SciterElement & gameDirectoryList, SCITER_ELEMENT source)
{
    SciterElement target = source, directoryItem;
    while (target.IsValid() && target != gameDirectoryList)
    {
        std::string className = target.GetAttribute("class");
        if (!className.empty())
        {
            std::string searchIn = " " + className + " ";
            if (searchIn.find(" directory-item ") != std::string::npos)
            {
                directoryItem = target;
                break;
            }
        }
        target = target.GetParent();
    }

    if (target.IsValid())
    {
        for (uint32_t i = 0, n = gameDirectoryList.GetChildCount(); i < n; i++)
        {
            SciterElement(gameDirectoryList.GetChild(i)).SetState(0, SciterElement::STATE_CHECKED, true);
        }
        target.SetState(SciterElement::STATE_CHECKED, 0, true);
    }
}

void SystemConfigGameBrowser::AddGameDirectory(const std::string & directory)
{
    SciterElement gameDirectoryList = m_page.GetElementByID("gameDirectoryList");
    SciterElement item;
    item.Create("div", directory.c_str());
    item.SetAttribute("class", "directory-item");
    item.SetAttribute("data-dir", directory.c_str());
    gameDirectoryList.Insert(item, gameDirectoryList.GetChildCount());
}

void SystemConfigGameBrowser::RemoveSelectedDirectory()
{
    SciterElement gameDirectoryList = m_page.GetElementByID("gameDirectoryList");
    if (!gameDirectoryList.IsValid())
    {
        return;
    }

    for (uint32_t i = 0, n = gameDirectoryList.GetChildCount(); i < n; i++)
    {
        SciterElement item(gameDirectoryList.GetChild(i));
        if ((item.GetState() & SciterElement::STATE_CHECKED) != 0)
        {
            item.Detach();
            break;
        }
    }
}

void SystemConfigGameBrowser::SaveSetting(void)
{
    SciterElement gameDirectoryList = m_page.GetElementByID("gameDirectoryList");
    uiSettings.gameDirectories.clear();
    for (uint32_t i = 0, n = gameDirectoryList.GetChildCount(); i < n; i++)
    {
        SciterElement item(gameDirectoryList.GetChild(i));
        uiSettings.gameDirectories.push_back(item.GetAttribute("data-dir"));
    }
    SaveUISetting();
}
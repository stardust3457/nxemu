#pragma once
#include <common/path.h>
#include <string>
#include <vector>

typedef std::vector<std::string> Stringlist;

struct UISettings
{
    Stringlist recentFiles;
    Stringlist gameDirectories;
    Path languageDir;
    std::string languageDirValue;
    std::string languageBase;
    std::string languageCurrent;
    bool sciterConsole;
    int32_t gameBrowserMyGameIconSize;
    bool performVulkanCheck;
    bool hasBrokenVulkan;
    bool enableAllControllers;
};

extern UISettings uiSettings;

void SetupUISetting(void);
void SaveUISetting(void);
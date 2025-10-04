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
    bool performVulkanCheck;
    bool hasBrokenVulkan;
};

extern UISettings uiSettings;

void LoadUISetting(void);
void SaveUISetting(void);
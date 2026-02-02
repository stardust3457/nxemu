#pragma once

__interface ISciterUI;
__interface ISystemModules;
class SciterMainWindow;
class SystemModules;

static const char * IID_ROMBROWSER = "F68DFC0D-C86D-4810-97C6-48289FA650ED";

__interface IRomBrowser
{
    void PopulateAsync() = 0;
    void SetMainWindow(SciterMainWindow * window, ISystemModules * modules) = 0;
    void ClearItems() = 0;
};

bool Register_WidgetRomBrowser(ISciterUI & SciterUI);
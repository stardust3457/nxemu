#include "dynamic_library.h"
#ifdef _WIN32
#include <windows.h>
#else
#include <dlfcn.h>
#endif

DynLibHandle DynamicLibraryOpen(const char * path, bool showErrors)
{
    if (path == nullptr)
    {
        return nullptr;
    }
#ifdef _WIN32
    UINT lastErrorMode = SetErrorMode(showErrors ? 0 : SEM_FAILCRITICALERRORS);
    DynLibHandle lib = (DynLibHandle)LoadLibraryA(path);
    SetErrorMode(lastErrorMode);
#else
    (void)showErrors;
    DynLibHandle lib = (DynLibHandle)(dlopen(path, RTLD_NOW | RTLD_LOCAL));
#endif
    return lib;
}

void DynamicLibraryClose(DynLibHandle libHandle)
{
    if (libHandle == nullptr)
    {
        return;
    }

#ifdef _WIN32
    FreeLibrary((HMODULE)(libHandle));
#else
    dlclose(libHandle);
#endif
}

void * DynamicLibraryGetProc(DynLibHandle libHandle, const char * name)
{
    if (name == nullptr)
    {
        return nullptr;
    }

#ifdef _WIN32
    return GetProcAddress((HMODULE)libHandle, name);
#else
    return dlsym(libHandle, name);
#endif
}
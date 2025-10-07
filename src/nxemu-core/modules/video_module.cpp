#include "video_module.h"

VideoModule::VideoModule() :
    m_CreateVideo(dummyCreateVideo),
    m_DestroyVideo(dummyDestroyVideo)
{
}

void VideoModule::UnloadModule(void)
{
    m_CreateVideo = dummyCreateVideo;
    m_DestroyVideo = dummyDestroyVideo;
}

bool VideoModule::LoadFunctions(void)
{
    m_CreateVideo = (tyCreateVideo)DynamicLibraryGetProc(m_lib, "CreateVideo");
    m_DestroyVideo = (tyDestroyVideo)DynamicLibraryGetProc(m_lib, "DestroyVideo");

    bool res = true;
    if (m_CreateVideo == nullptr)
    {
        m_CreateVideo = dummyCreateVideo;
        res = false;
    }
    if (m_DestroyVideo == nullptr)
    {
        m_DestroyVideo = dummyDestroyVideo;
        res = false;
    }
    return res;
}

MODULE_TYPE VideoModule::ModuleType() const
{
    return MODULE_TYPE_VIDEO;
}

IVideo * VideoModule::dummyCreateVideo(IRenderWindow & /*RenderWindow*/, ISystemModules & /*modules*/)
{
    return nullptr;
}

void VideoModule::dummyDestroyVideo(IVideo * /*Video*/)
{
}

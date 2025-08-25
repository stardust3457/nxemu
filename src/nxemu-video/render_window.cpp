#include "render_window.h"
#include "yuzu_video_core/frontend/graphics_context.h"
#include "yuzu_common/settings.h"
#include <Windows.h>
#include <glad/glad.h>
#include <nxemu-module-spec/video.h>
#include <nxemu-core/settings/identifiers.h>

extern IModuleSettings * g_settings;

class OpenGLSharedContext : public Core::Frontend::GraphicsContext
{
public:
    explicit OpenGLSharedContext(IRenderWindow & renderWindow) :
        m_renderWindow(renderWindow),
        m_glrc(nullptr),
        m_hdc(nullptr)
    {
        typedef HGLRC(WINAPI * PFNWGLCREATECONTEXTATTRIBSARBPROC)(HDC hDC, HGLRC hShareContext, const int * attribList);

#define WGL_CONTEXT_MAJOR_VERSION_ARB 0x2091
#define WGL_CONTEXT_MINOR_VERSION_ARB 0x2092
#define WGL_CONTEXT_PROFILE_MASK_ARB 0x9126
#define WGL_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB 0x00000002

        HWND hWnd = (HWND)renderWindow.RenderSurface();
        if (hWnd == nullptr)
        {
            return;
        }
        m_hdc = GetDC(hWnd);
        if (m_hdc == nullptr)
        {
            return;
        }

        int pfm = GetPixelFormat(m_hdc);
        if (pfm == 0)
        {
            PIXELFORMATDESCRIPTOR pfd = { 0 };
            pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
            pfd.nVersion = 1;
            pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_GENERIC_ACCELERATED | PFD_DOUBLEBUFFER;
            pfd.iPixelType = PFD_TYPE_RGBA;
            pfd.iLayerType = PFD_MAIN_PLANE;
            pfd.cColorBits = 32;
            pfd.cDepthBits = 24;
            pfd.cAuxBuffers = 1;

            int pfm = ChoosePixelFormat(m_hdc, &pfd);
            if (pfm == 0)
            {
                pfd.cAuxBuffers = 0;
                pfm = ChoosePixelFormat(m_hdc, &pfd);
            }
            if (pfm == 0)
            {
                return;
            }
            if (SetPixelFormat(m_hdc, pfm, &pfd) == 0)
            {
                return;
            }
        }

        m_glrc = wglCreateContext(m_hdc);
        if (m_glrc == nullptr)
        {
            return;
        }
    }
    ~OpenGLSharedContext()
    {
        DoneCurrent();
        if (m_hdc != nullptr)
        {
            HWND hwnd = static_cast<HWND>(m_renderWindow.RenderSurface());
            ReleaseDC(hwnd, m_hdc); 
            m_hdc = nullptr;
        }
    }
    void SwapBuffers() override
    {
        ::SwapBuffers(m_hdc);
    }
    void MakeCurrent() override
    {
        wglMakeCurrent(m_hdc, m_glrc);
    }
    void DoneCurrent() override
    {
        wglMakeCurrent(nullptr, nullptr);
    }
    IRenderWindow & m_renderWindow;
    HGLRC m_glrc;
    HDC m_hdc;
};

class DummyContext : public Core::Frontend::GraphicsContext
{
};

RenderWindow::RenderWindow(IRenderWindow & renderWindow) :
    m_renderWindow(renderWindow),
    m_firstFrame(false)
{
#ifdef WIN32
    window_info.type = Core::Frontend::WindowSystemType::Windows;
#endif
    window_info.render_surface = renderWindow.RenderSurface();
    NotifyClientAreaSizeChanged({ 0,0 });
    UpdateCurrentFramebufferLayout(640, 480);

    if (Settings::values.renderer_backend.GetValue() == Settings::RendererBackend::OpenGL)
    {
        LoadOpenGL();
    }
}

void RenderWindow::OnFrameDisplayed()
{
    if (!m_firstFrame)
    {
        m_firstFrame = true;
        g_settings->SetBool(NXCoreSetting::DisplayedFrames, true);
    }
}

std::unique_ptr<Core::Frontend::GraphicsContext> RenderWindow::CreateSharedContext() const
{
    if (Settings::values.renderer_backend.GetValue() == Settings::RendererBackend::OpenGL)
    {
        return std::make_unique<OpenGLSharedContext>(m_renderWindow);
    }
    return std::make_unique<DummyContext>();
}

bool RenderWindow::IsShown() const
{
    return true;
}

void RenderWindow::LoadOpenGL()
{
    auto context = CreateSharedContext();
    auto scope = context->Acquire();
    gladLoadGL();
}

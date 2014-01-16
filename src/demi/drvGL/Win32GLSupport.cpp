
#include "Win32GLSupport.h"
#include <sstream>
#include <gl/wglew.h>

GLenum wglewContextInit(Demi::DiGLSupport *glSupport);

namespace Demi
{
    LRESULT CALLBACK DummyWndProc(HWND hwnd, UINT umsg, WPARAM wp, LPARAM lp)
    {
        return DefWindowProc(hwnd, umsg, wp, lp);
    }

    template<class C> 
    void remove_duplicates(C& c)
    {
        std::sort(c.begin(), c.end());
        typename C::iterator p = std::unique(c.begin(), c.end());
        c.erase(p, c.end());
    }

    DiWin32GLSupport::DiWin32GLSupport()
        : mHasPixelFormatARB(false)
        , mHasMultisample(false)
        , mHasHardwareGamma(false)
        , mHDC(NULL)
    {
        InitWGL();
    }

    DiWin32GLSupport::~DiWin32GLSupport()
    {

    }

    void DiWin32GLSupport::InitWGL()
    {
#ifdef DEMI_STATIC_API
        HINSTANCE hinst = GetModuleHandle(NULL);
#else
#   ifdef NDEBUG
        HINSTANCE hinst = GetModuleHandle("DiDrvGL.dll");
#   else
        HINSTANCE hinst = GetModuleHandle("DiDrvGL_d.dll");
#   endif
#endif

        LPCSTR dummyText = "_WglDummy";
        WNDCLASS dummyClass;
        memset(&dummyClass, 0, sizeof(WNDCLASS));
        dummyClass.style = CS_OWNDC;
        dummyClass.hInstance = hinst;
        dummyClass.lpfnWndProc = DummyWndProc;
        dummyClass.lpszClassName = dummyText;
        RegisterClass(&dummyClass);

        HWND hwnd = CreateWindow(dummyText, dummyText,
            WS_POPUP | WS_CLIPCHILDREN,
            0, 0, 32, 32, 0, 0, hinst, 0);

        if (hwnd == NULL)
        {
            DI_ERROR("Cannot create a dummy window, InitWGL()");
            return;
        }

        HDC hdc = GetDC(hwnd);
        PIXELFORMATDESCRIPTOR pfd;
        memset(&pfd, 0, sizeof(PIXELFORMATDESCRIPTOR));
        pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
        pfd.nVersion = 1;
        pfd.cColorBits = 16;
        pfd.cDepthBits = 15;
        pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
        pfd.iPixelType = PFD_TYPE_RGBA;

        int format;
        if ((format = ChoosePixelFormat(hdc, &pfd)) != 0)
            SetPixelFormat(hdc, format, &pfd);

        HGLRC hrc = wglCreateContext(hdc);
        if (hrc)
        {
            HGLRC oldrc = wglGetCurrentContext();
            HDC oldhdc = wglGetCurrentDC();
            wglMakeCurrent(hdc, hrc);

            PFNWGLGETEXTENSIONSSTRINGARBPROC _wglGetExtensionsStringARB =
                (PFNWGLGETEXTENSIONSSTRINGARBPROC)
                wglGetProcAddress("wglGetExtensionsStringARB");

            if (_wglGetExtensionsStringARB)
            {
                std::istringstream wglexts(_wglGetExtensionsStringARB(hdc));
                std::string ext;
                while (wglexts >> ext)
                {
                    if (ext == "WGL_ARB_pixel_format")
                        mHasPixelFormatARB = true;
                    else if (ext == "WGL_ARB_multisample")
                        mHasMultisample = true;
                    else if (ext == "WGL_EXT_framebuffer_sRGB")
                        mHasHardwareGamma = true;
                }
            }

            if (mHasPixelFormatARB && mHasMultisample)
            {
                // enumerate all formats w/ multisampling
                static const int iattr[] = {
                    WGL_DRAW_TO_WINDOW_ARB, GL_TRUE,
                    WGL_SUPPORT_OPENGL_ARB, GL_TRUE,
                    WGL_DOUBLE_BUFFER_ARB, GL_TRUE,
                    WGL_SAMPLE_BUFFERS_ARB, GL_TRUE,
                    WGL_ACCELERATION_ARB, WGL_FULL_ACCELERATION_ARB,
                    /* We are no matter about the colour, depth and stencil buffers here
                    WGL_COLOR_BITS_ARB, 24,
                    WGL_ALPHA_BITS_ARB, 8,
                    WGL_DEPTH_BITS_ARB, 24,
                    WGL_STENCIL_BITS_ARB, 8,
                    */
                    WGL_SAMPLES_ARB, 2,
                    0
                };
                int formats[256];
                unsigned int count;

                WGLEW_GET_FUN(__wglewChoosePixelFormatARB) =
                    (PFNWGLCHOOSEPIXELFORMATARBPROC)wglGetProcAddress("wglChoosePixelFormatARB");
                if (WGLEW_GET_FUN(__wglewChoosePixelFormatARB)(hdc, iattr, 0, 256, formats, &count))
                {
                    int query = WGL_SAMPLES_ARB, samples;
                    for (unsigned int i = 0; i < count; ++i)
                    {
                        PFNWGLGETPIXELFORMATATTRIBIVARBPROC _wglGetPixelFormatAttribivARB =
                            (PFNWGLGETPIXELFORMATATTRIBIVARBPROC)
                            wglGetProcAddress("wglGetPixelFormatAttribivARB");
                        if (_wglGetPixelFormatAttribivARB(hdc, formats[i], 0, 1, &query, &samples))
                        {
                            mFSAALevels.push_back(samples);
                        }
                    }
                    remove_duplicates(mFSAALevels);
                }
            }

            wglMakeCurrent(oldhdc, oldrc);
            wglDeleteContext(hrc);
        }

        // clean up our dummy window and class
        DestroyWindow(hwnd);
        UnregisterClass(dummyText, hinst);
    }

    void DiWin32GLSupport::InitExtensions()
    {
        DiGLSupport::InitExtensions();

        DI_ASSERT(mHDC);
        
        wglewContextInit(this);

        PFNWGLGETEXTENSIONSSTRINGARBPROC _wglGetExtensionsStringARB =
            (PFNWGLGETEXTENSIONSSTRINGARBPROC)wglGetProcAddress("wglGetExtensionsStringARB");
        if (!_wglGetExtensionsStringARB)
            return;

        const char *wgl_extensions = _wglGetExtensionsStringARB(mHDC);
        
        std::stringstream ext;
        std::string instr;
        ext << wgl_extensions;
        DI_LOG("Supported GL extension : ");
        while (ext >> instr)
        {
            DI_LOG(instr.c_str());
            mExtensionList.insert(instr.c_str());
        }
    }

    DiString DiWin32GLSupport::TranslateWGLError()
    {
        int winError = GetLastError();
        char* errDesc;
        int i;

        errDesc = new char[255];
        i = FormatMessage(
            FORMAT_MESSAGE_FROM_SYSTEM |
            FORMAT_MESSAGE_IGNORE_INSERTS,
            NULL,
            winError,
            MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
            (LPTSTR)errDesc,
            255,
            NULL
            );

        return DiString(errDesc);
    }

}
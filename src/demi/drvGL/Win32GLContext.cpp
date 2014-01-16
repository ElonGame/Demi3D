
#include "Win32GLContext.h"
#include "Win32GLUtil.h"

namespace Demi
{
    DiWin32GLContext::DiWin32GLContext(HDC hdc, HGLRC glrc)
        :mHDC(hdc), mGLRc(glrc)
    {
    }

    DiWin32GLContext::DiWin32GLContext(DiWndHandle wnd)
    {
        InitFromHwnd((HWND)wnd);
    }

    DiWin32GLContext::~DiWin32GLContext()
    {
    }

    void DiWin32GLContext::BeginContext()
    {
        wglMakeCurrent(mHDC, mGLRc);
    }

    void DiWin32GLContext::EndContext()
    {
        wglMakeCurrent(NULL, NULL);
    }

    void DiWin32GLContext::Release()
    {
        if (mGLRc)
        {
            wglDeleteContext(mGLRc);
            mGLRc = NULL;
            mHDC = NULL;
        }
    }

    DiGLContext* DiWin32GLContext::Clone() const
    {
        HGLRC newCtx = wglCreateContext(mHDC);

        if (!newCtx)
        {
            DI_WARNING("Error calling wglCreateContext");
            return nullptr;
        }

        HGLRC oldrc = wglGetCurrentContext();
        HDC oldhdc = wglGetCurrentDC();
        wglMakeCurrent(NULL, NULL);
        
        if (!wglShareLists(mGLRc, newCtx))
        {
            DiString errorMsg = DiWin32GLUtil::TranslateWGLError();
            wglDeleteContext(newCtx);
            DI_WARNING("Error calling wglShareLists : %s", errorMsg.c_str());
            return nullptr;
        }

        wglMakeCurrent(oldhdc, oldrc);
        return DI_NEW DiWin32GLContext(mHDC, newCtx);
    }

    void DiWin32GLContext::InitFromHwnd(HWND hwnd)
    {
        HDC old_hdc = wglGetCurrentDC();
        HGLRC old_context = wglGetCurrentContext();

        mHDC = GetDC(hwnd);

        // select valid pixel format
        

        mGLRc = wglCreateContext(mHDC);
        if (!mGLRc)
        {
            DI_LOG(DiWin32GLUtil::TranslateWGLError().c_str());
            DI_ERROR("Cannot create wgl context.");
            return;
        }

        if (old_context && old_context != mGLRc)
        {
            // Share lists with old context
            if (!wglShareLists(old_context, mGLRc))
            {
                DI_ERROR("Cannot share wgl lists");
            }
        }

        if (!wglMakeCurrent(mHDC, mGLRc))
        {
            DI_ERROR("Failed to call wglMakeCurrent");
        }

        PFNWGLSWAPINTERVALEXTPROC _wglSwapIntervalEXT =
            (PFNWGLSWAPINTERVALEXTPROC)wglGetProcAddress("wglSwapIntervalEXT");

        // TODO: VSYNC
        //if (_wglSwapIntervalEXT)
            //_wglSwapIntervalEXT(mVSync ? mVSyncInterval : 0);

        if (old_context && old_context != mGLRc)
        {
            // Restore old context
            if (!wglMakeCurrent(mHDC, mGLRc))
            {
                DI_ERROR("Failed to call wglMakeCurrent");
            }
        }
    }
}
/**********************************************************************
This source file is a part of Demi3D
   __  ___  __  __  __
  |  \|_ |\/||   _)|  \ 
  |__/|__|  ||  __)|__/ 

Copyright (c) 2013-2014 Demi team
https://github.com/wangyanxing/Demi3D

Released under the MIT License
https://github.com/wangyanxing/Demi3D/blob/master/License.txt
***********************************************************************/
#include "DrvGLPch.h"
#include "Win32GLContext.h"
#include "Win32GLUtil.h"

namespace Demi
{
    DiEGLContext::DiEGLContext(DiEGLUtil* glutil, DiWndHandle wnd)
        : mGLUtil(glutil)
    {
        InitFromHwnd((HWND)wnd);
        if (!glutil->GetMainHDC())
            glutil->SetMainHDC(mHDC);
    }

    DiEGLContext::DiEGLContext(DiEGLUtil* glutil, HDC dc, HGLRC glrc)
        : mGLUtil(glutil),
        mHDC(dc),
        mGLRc(glrc)
    {
        if (!glutil->GetMainHDC())
            glutil->SetMainHDC(mHDC);
    }

    DiEGLContext::~DiEGLContext()
    {
    }

    void DiEGLContext::BeginContext()
    {
        wglMakeCurrent(mHDC, mGLRc);
    }

    void DiEGLContext::EndContext()
    {
        wglMakeCurrent(NULL, NULL);
    }

    void DiEGLContext::Release()
    {
        if (mGLRc)
        {
            wglDeleteContext(mGLRc);
            mGLRc = NULL;
            mHDC = NULL;
        }
    }

    DiGLContext* DiEGLContext::Clone() const
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
            DiString errorMsg = DiEGLUtil::TranslateWGLError();
            wglDeleteContext(newCtx);
            DI_WARNING("Error calling wglShareLists : %s", errorMsg.c_str());
            return nullptr;
        }

        wglMakeCurrent(oldhdc, oldrc);
        return DI_NEW DiEGLContext(mGLUtil, mHDC, newCtx);
    }

    void DiEGLContext::InitFromHwnd(HWND hwnd)
    {
        HDC old_hdc = wglGetCurrentDC();
        HGLRC old_context = wglGetCurrentContext();

        mHDC = GetDC(hwnd);

        // select valid pixel format
        // todo
        bool formatOk = mGLUtil->SelectPixelFormat(mHDC, 32, 0, false); // TODO!
        if (!formatOk)
        {
            DI_ERROR("Invalid pixel format!");
            return;
        }

        mGLRc = wglCreateContext(mHDC);
        if (!mGLRc)
        {
            DI_INFO(DiEGLUtil::TranslateWGLError().c_str());
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
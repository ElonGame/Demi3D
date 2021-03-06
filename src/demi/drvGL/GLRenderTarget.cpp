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
#include "GLRenderTarget.h"
#include "GLDepthBuffer.h"
#include "GLDriver.h"
#include "GLTypeMappings.h"
#include "GLFrameBuffer.h"
#include "GLRenderBuffer.h"
#include "GLContext.h"

#include "Texture.h"
#include "Window.h"

namespace Demi
{
    DiGLRenderTarget::DiGLRenderTarget()
        :mGLFormat(0),
        mFrameBuffer(nullptr)
    {
    }

    DiGLRenderTarget::~DiGLRenderTarget()
    {
        if (mFrameBuffer)
        {
            DI_DELETE mFrameBuffer;
            mFrameBuffer = nullptr;
        }
    }

    bool DiGLRenderTarget::BindRenderTarget(uint8 mrtid)
    {
        if (mFrameBuffer)
        {
            mFrameBuffer->Bind();
        }
        else
        {
            //DI_DEBUG("Binding Framebuffer");
            glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
        }
        return true;
    }

    bool DiGLRenderTarget::BindDepthStencil()
    {
        DiGLDepthBuffer *depthBuffer = static_cast<DiGLDepthBuffer*>(GetDepthBuffer());

        if (GetDepthBufferPool() != DiDepthBuffer::POOL_NO_DEPTH &&
            (!depthBuffer || depthBuffer->GetGLContext() != static_cast<DiGLDriver*>(Driver)->GetCurrentContext()))
        {
            //Depth is automatically managed and there is no depth buffer attached to this RT
            //or the Current context doesn't match the one this Depth buffer was created with
            Driver->SetDepthBufferFor(this);
        }
        return true;
    }

    void DiGLRenderTarget::AttachSurface()
    {
        mGLFormat = DiGLTypeMappings::GetClosestGLInternalFormat(mParentTex->GetFormat());

        mFrameBuffer->AttachSurface(0, mParentTex);
    }

    void DiGLRenderTarget::DetachSurface()
    {
        mFrameBuffer->DetarchSurface(0);
    }

    DiDepthBuffer* DiGLRenderTarget::CreateDepthBuffer()
    {
        DiDepthBuffer *ret = nullptr;

        if (!mFrameBuffer) 
            return ret;

        GLuint depthFormat, stencilFormat;
        DiGLDriver::FBOManager->GetBestDepthStencil(mFrameBuffer->GetFormat(), &depthFormat, &stencilFormat);

        DiGLRenderBuffer* db = DI_NEW DiGLRenderBuffer(depthFormat, GetWidth(), GetHeight());
        DiGLRenderBuffer* sb = db;

        if (depthFormat != GL_DEPTH24_STENCIL8_EXT && sb)
        {
            sb = DI_NEW DiGLRenderBuffer(stencilFormat, GetWidth(), GetHeight());
        }

        auto context = static_cast<DiGLDriver*>(Driver)->GetCurrentContext();
        return DI_NEW DiGLDepthBuffer(0, GetWidth(), GetHeight(), context, db, sb, 0, 0, false);
    }

    void DiGLRenderTarget::PreBind()
    {
        uint32 left   = uint32(mViewport.mLeft   * mWidth);
        uint32 top    = uint32(mViewport.mTop    * mHeight);
        uint32 width  = uint32(mViewport.mWidth  * mWidth);
        uint32 height = uint32(mViewport.mHeight * mHeight);

        if (!mFlippingUV)
        {
            // Convert "upper-left" corner to "lower-left"
            //top = GetHeight() - height - top;
        }

        Driver->SetViewport(left, top, width, height);
    }

    bool DiGLRenderTarget::IsCompatibleWith(DiDepthBuffer* db)
    {
        bool retVal = false;

        if (mWidth != db->GetWidth() || mHeight != db->GetHeight() )
            return retVal;

        DiGLDepthBuffer* gldb = static_cast<DiGLDepthBuffer*>(db);
        DiGLRenderBuffer* glDepth = gldb->GetDepthBuffer();
        DiGLRenderBuffer* glStencil = gldb->GetStencilBuffer();

        if (!mFrameBuffer)
        {
            DiGLContext* dbContext = gldb->GetGLContext();
            if (!glDepth && !glStencil && dbContext == GetContext())
            {
                retVal = true;
            }
        }
        else
        {
            if (glDepth || glStencil)
            {
                GLenum depthFormat, stencilFormat;
                static_cast<DiGLDriver*>(Driver)->GetDepthStencilFormatFor(mFrameBuffer->GetFormat(),
                                                                           &depthFormat, &stencilFormat);

                bool bSameDepth = false;

                if (glDepth)
                    bSameDepth |= glDepth->GetGLFormat() == depthFormat;

                bool bSameStencil = false;

                if (!glStencil || glStencil == glDepth)
                    bSameStencil = stencilFormat == GL_NONE;
                else
                {
                    if (glStencil)
                        bSameStencil = stencilFormat == glStencil->GetGLFormat();
                }

                retVal = bSameDepth && bSameStencil;
            }
        }
        
        return retVal;
    }

    bool DiGLRenderTarget::AttachDepthBuffer(DiDepthBuffer *depthBuffer)
    {
        bool result;
        if ((result = DiRenderTarget::AttachDepthBuffer(depthBuffer)))
        {
            if (mFrameBuffer)
                mFrameBuffer->AttachDepthBuffer(depthBuffer);
        }

        return result;
    }

    void DiGLRenderTarget::Init()
    {
        mFrameBuffer = DI_NEW DiGLFrameBuffer();
    }

    void DiGLRenderTarget::DetachDepthBuffer()
    {
        if (mFrameBuffer)
            mFrameBuffer->DetachDepthBuffer();
        DiRenderTarget::DetachDepthBuffer();
    }

    DiGLWindowTarget::DiGLWindowTarget()
        : mWnd(nullptr)
        , mContext(nullptr)
    {
    }

    DiGLWindowTarget::~DiGLWindowTarget()
    {
    }

    void DiGLWindowTarget::Create(DiWndHandle wnd, DiGLContext* context)
    {
        mWnd = wnd;
        mContext = context;

        Driver->GetWindowSize(wnd, mWidth, mHeight);

        DiGLDepthBuffer *depthBuffer = DI_NEW DiGLDepthBuffer(DiDepthBuffer::POOL_DEFAULT, 
            mWidth, mHeight,
            static_cast<DiGLDriver*>(Driver)->GetCurrentContext(), nullptr, nullptr,
            0, 0, true);

        AttachDepthBuffer(depthBuffer);
    }

    bool DiGLWindowTarget::SwapBuffer()
    {
        DiWindow* wnd = Driver->GetWindowManager()->GetWindow(mWnd);
        if(wnd)
        {
            bool resized = CheckSizeChanged(wnd);
            wnd->SwapBuffers();
            return !resized;
        }
        else
            return false;
    }

    void DiGLWindowTarget::Init()
    {
    }

    bool DiGLWindowTarget::CheckSizeChanged(DiWindow* wnd)
    {
        uint32 w, h;
        Driver->GetWindowSize(wnd->GetWndHandle(), w, h);
        if (w != mWidth || h != mHeight)
        {
            return true;
        }
        return false;
    }

    void DiGLWindowTarget::OnDeviceLost()
    {

    }

    void DiGLWindowTarget::OnDeviceReset()
    {
        Driver->GetWindowSize(mWnd, mWidth, mHeight);
    }

}
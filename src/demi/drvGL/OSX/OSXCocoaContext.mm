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
#include "OSX/OSXCocoaContext.h"
#include "OSX/OSXGLUtil.h"

namespace Demi
{
    DiOSXCocoaContext::DiOSXCocoaContext(NSOpenGLContext *context,
                                         NSOpenGLPixelFormat *pixelFormat)
        :mNSGLContext(context), mNSGLPixelFormat(pixelFormat)
    {
		[mNSGLPixelFormat retain];
    }
    
    DiOSXCocoaContext::~DiOSXCocoaContext()
    {
		//_unregisterContext();
		
		[mNSGLPixelFormat release];
    }
    
    void DiOSXCocoaContext::BeginContext()
    {
		[mNSGLContext makeCurrentContext];
    }
    
    void DiOSXCocoaContext::EndContext()
    {
        [NSOpenGLContext clearCurrentContext];
    }
    
    void DiOSXCocoaContext::Release()
    {
    }
    
    DiGLContext* DiOSXCocoaContext::Clone() const
    {
        NSOpenGLContext *cloneCtx = [[NSOpenGLContext alloc] initWithFormat:mNSGLPixelFormat shareContext:mNSGLContext];
		[cloneCtx copyAttributesFromContext:mNSGLContext withMask:GL_ALL_ATTRIB_BITS];
		return DI_NEW DiOSXCocoaContext(cloneCtx, mNSGLPixelFormat);
    }
    
    NSOpenGLContext* DiOSXCocoaContext::GetContext()
    {
        return mNSGLContext;
    }
    
    NSOpenGLPixelFormat* DiOSXCocoaContext::GetPixelFormat()
    {
        return mNSGLPixelFormat;
    }
}
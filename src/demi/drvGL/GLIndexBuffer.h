
/********************************************************************
    File:       GLIndexBuffer.h
    Creator:    demiwangya
*********************************************************************/

#pragma once

#include "IndexBuffer.h"

namespace Demi
{
    class DI_GLDRV_API DiGLIndexBuffer : public DiIndexBuffer
    {
    public:

        DiGLIndexBuffer();

        ~DiGLIndexBuffer();

    public:

        void        Create(uint32 size, DiIndexBufType type = IB_16BITS, uint32 usage = RU_STATIC);

        void        Bind();

        void        Release();

        void        OnDeviceLost();

        void        OnDeviceReset();

        void*       Lock(uint32 offset, uint32 size, DiLockFlag flag = LOCK_NORMAL);

        void        Unlock();

    private:

        void        ReadData(uint32 offset, uint32 length, void* pDest);
        
        void        WriteData(uint32 offset, uint32 length,
                        const void* pSource, bool discardWholeBuffer = false);

    private:

        GLuint      mBufferId;
        
        bool        mLockedToScratch;
        
        size_t      mScratchOffset;
        
        size_t      mScratchSize;
        
        void*       mScratchPtr;

        bool        mScratchUploadOnUnlock;
    };
}
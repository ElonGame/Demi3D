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

#include "GfxPch.h"
#include "Image.h"
#include "AssetManager.h"

#include "nv_dds.h"
#include "stb_image.h"
#include "stb_image_write.h"
#include "PVRTC.h"

namespace Demi 
{
    static uint32 ComputeDDSDimension(uint32 dimension)
    {
        if(dimension < 4)
            dimension = 4;
        else
        {
            uint32 mod = dimension % 4;
            if(mod) dimension += 4 - mod;
        }
        return dimension;
    }

    //////////////////////////////////////////////////////////////////////////

    DiImage::DiImage( const DiString& name ):
        mWidth(0)
        , mHeight(0)

    {
        mImageData = DiAssetManager::GetInstance().OpenArchive(name);
    }

    DiImage::DiImage( DiDataStreamPtr stream ):
        mWidth(0)
        , mHeight(0)
        , mImageData(stream)
    {
    }

    DiImage::~DiImage()
    {

    }
    
    bool DiImage::SaveTextureAsPng(DiTexturePtr texture, const DiString& file)
    {
        DiPixelFormat fmt = texture->GetFormat();
        if(DiPixelBox::IsCompressedFormat(fmt))
        {
            DI_WARNING("Cannot save compressed texture to file");
            return false;
            
        }
        
        uint32 width = texture->GetWidth();
        uint32 height = texture->GetHeight();
        
        // TODO
        uint32 components = 4;//DiPixelBox::GetNumComponents(fmt);
        uint32 size = DiPixelBox::ComputeImageByteSize(width, height, fmt);
        
        uint8* data = DI_NEW uint8[size];
        DiPixelBox dst(width, height, fmt, data);
        
        texture->CopyToMemory(dst);
        
        // Swizzle RGBA -> BGRA
        uint32 *ptr = (uint32*)data;
        for (uint32 i = 0, si = width * height; i < si; ++i)
        {
            uint32 col = *ptr;
            *ptr++ = (col & 0xFF00FF00) | ((col & 0x000000FF) << 16) | ((col & 0x00FF0000) >> 16);
        }
        
        bool ret = stbi_write_png(file.c_str(), width, height, components, data, 0) != 0;
        DI_DELETE[] data;
        return ret;
    }
    
    bool DiImage::LoadRawDataToTexture(DiTexture* texture, DiPixelFormat fmt, uint32 width, uint32 height)
    {
        if(!mImageData)
            return false;
        
        size_t size = mImageData->Size();
        if(size == 0)
            return false;
        
        shared_ptr<uint8> buffer(DI_NEW uint8[size], [](uint8 *p) { DI_DELETE[] p; });
        mImageData->Read(buffer.get(), size);
        
        texture->Release();
        texture->SetDimensions(width, height);
        texture->SetFormat(fmt);
        texture->SetNumLevels(1);
        texture->SetResourceUsage(RU_WRITE_ONLY);
        texture->CreateTexture();
        DiTextureDrv* texDrv = texture->GetTextureDriver();
        
        DiPixelBox pixbox(width, height, fmt, buffer.get());
        texDrv->CopyFromMemory(pixbox, 0, 0);
        
        return true;
    }

    bool DiImage::LoadToTexture( DiTexture* texture )
    {
        if (!mImageData)
            return false;

        DiString head = "head";
        
        int t = mImageData->Tell();
        mImageData->Read(&head[0], 4);
        t = mImageData->Tell();
        mImageData->Seek(0);

        if (head == "DDS ")
        {
            ParseDDS(texture);
        }
        else if(DiPVRTC::CheckHead(head.c_str()))
        {
            ParsePVR(texture);
        }
        else
        {
            //tga/bmp/jpg/png/hdr..
            ParseOthers(texture);
        }

        return true;
    }
    
    void DiImage::ParsePVR(DiTexture* texture)
    {
        DI_INFO("Loading pvr image : %s", mImageData->GetName().c_str());
     
        DiPVRTC pvr;
        bool ok = pvr.Load(mImageData);
        DI_ASSERT(ok);
        
        if(ok)
        {
            if(texture)
            {
                texture->Release();
                texture->SetTextureType(pvr.IsCubeMap() ? TEXTURE_CUBE : TEXTURE_2D);
                texture->SetDimensions(pvr.mWidth, pvr.mHeight);
                texture->SetFormat(pvr.mFormat);
                texture->SetResourceUsage(RU_WRITE_ONLY);
                texture->SetNumLevels(pvr.mMipmaps);
                
                if (texture->GetNumLevels() > 1)
                    texture->SetAutoMipmap(false);
                
                texture->CreateTexture();
                DiTextureDrv* texDrv = texture->GetTextureDriver();
                
                int faces = pvr.mNumFaces;
                DI_ASSERT(faces == 1 || faces == 6);
                
                for (int f = 0; f < faces; f++)
                {
                    for (uint32 i = 0; i < texture->GetNumLevels(); i++)
                    {
                        auto pixbox = pvr.GetPixelBox(f, i);
                        texDrv->CopyFromMemory(pixbox, i, f);
                    }
                }
            }

        }
    }

    void DiImage::ParseDDS(DiTexture* texture)
    {
        DI_INFO("Loading dds image : %s", mImageData->GetName().c_str());

        nv_dds::CDDSImage ddsimage;
        bool ok = ddsimage.load(mImageData.get(), false);
        DI_ASSERT(ok);

        if(ok)
        {
            nv_dds::TextureFormat ddsformat = ddsimage.get_format();
            DiPixelFormat format = PIXEL_FORMAT_MAX;
            switch(ddsformat)
            {
            case nv_dds::TextureBGR:          format = PF_R8G8B8;      break;
            case nv_dds::TextureBGRA:         format = PF_A8R8G8B8;    break;
            case nv_dds::TextureDXT1:         format = PF_DXT1;        break;
            case nv_dds::TextureDXT3:         format = PF_DXT3;        break;
            case nv_dds::TextureDXT5:         format = PF_DXT5;        break;
            case nv_dds::TextureLuminance:    format = PF_L8;          break;
            default:
                DI_WARNING("Unsupported texture pixel format");
            }

            if(texture)
            {
                texture->Release();
                texture->SetTextureType(ddsimage.is_cubemap() ? TEXTURE_CUBE : TEXTURE_2D);
                texture->SetDimensions(ddsimage.get_width(),ddsimage.get_height());
                texture->SetFormat(format);
                texture->SetResourceUsage(RU_WRITE_ONLY);
                texture->SetNumLevels(ddsimage.get_num_mipmaps()+1);

                if (texture->GetNumLevels() > 1)
                    texture->SetAutoMipmap(false);

                texture->CreateTexture();
                DiTextureDrv* texDrv = texture->GetTextureDriver();

                int faces = ddsimage.is_cubemap()?6:1;

                for (int f = 0; f < faces; f++)
                {
                    void* levelSrc = nullptr;
                    if (ddsimage.is_cubemap())
                        levelSrc = (void*)ddsimage.get_cubemap_face(f);
                    else
                        levelSrc = (void*)ddsimage;

                    const uint32 levelWidth = texture->GetWidth();
                    const uint32 levelHeight = texture->GetHeight();

                    DiPixelBox pixbox(levelWidth, levelHeight, format, levelSrc);
                    texDrv->CopyFromMemory(pixbox, 0, f);

                    for (uint32 i = 1; i < texture->GetNumLevels(); i++)
                    {
                        const nv_dds::CSurface &surface = ddsimage.get_mipmap(i - 1, f);
                        void* curLevelSrc = (void*)surface;
                        const uint32 curLevelWidth = surface.get_width();
                        const uint32 curLevelHeight = surface.get_height();
                        DiPixelBox pb(curLevelWidth, curLevelHeight, format, curLevelSrc);
                        texDrv->CopyFromMemory(pb, i, f);
                    }
                }
            }
        }
    }

    uint8* DiImage::_LoadImage(int& width, int& height, int& components)
    {
        int size = (int)mImageData->Size();
        shared_ptr<uint8> buffer(DI_NEW uint8[size], [](uint8 *p) { DI_DELETE[] p; });
        mImageData->Read(buffer.get(), size);
        uint8* data = buffer.get();
        
        bool hdr = false;
        if( stbi_is_hdr_from_memory( (unsigned char *)data, size ) > 0 )
            hdr = true;
        
        void *pixels = nullptr;
        if(hdr)
            pixels = stbi_loadf_from_memory( data, size, &width, &height, &components, 4 );
        else
            pixels = stbi_load_from_memory( data, size, &width, &height, &components, 4 );
        
        if(!pixels)
        {
            DI_WARNING( "Invalid image format %s", stbi_failure_reason());
            return nullptr;
        }
        
        // Swizzle RGBA -> BGRA
        uint32 *ptr = (uint32*)pixels;
        for (uint32 i = 0, si = width * height; i < si; ++i)
        {
            uint32 col = *ptr;
            *ptr++ = (col & 0xFF00FF00) | ((col & 0x000000FF) << 16) | ((col & 0x00FF0000) >> 16);
        }
        return (uint8*)pixels;
    }

    void DiImage::_FreeImage(uint8* data)
    {
        if (data)
            stbi_image_free(data);
    }

    void DiImage::ParseOthers(DiTexture* texture)
    {
        int width = 0, height = 0;
        int components = 0;
        uint8* data = _LoadImage(width, height, components);
        if (!data)
        {
            DI_WARNING("Could not load image %s: %s", mImageData->GetName().c_str(), stbi_failure_reason());
            return;
        }

        DI_INFO("Loading texture: %s", mImageData->GetName().c_str());
        
        components = 4;
        DiPixelFormat fmt = PF_A8R8G8B8;
        if(components == 1)
            fmt = PF_A8;
        else if(components == 3)
            fmt = PF_R8G8B8;
        else if(components == 4)
            fmt = PF_A8R8G8B8;
        else
        {
            DI_WARNING("Component number of %d currently isn't supported yet, failed to load the texture");
        }

        mWidth  = width;
        mHeight = height;
        uint32 maxmipmap = DiPixelBox::GetMaxMipmaps(width, height);
        
        if (texture)
        {
            texture->Release();
            texture->SetDimensions(width, height);
            texture->SetFormat(fmt);
            texture->SetNumLevels(maxmipmap);
            texture->SetResourceUsage(RU_WRITE_ONLY);
            texture->CreateTexture();
            DiTextureDrv* texDrv = texture->GetTextureDriver();

            DiPixelBox pixbox(width, height, fmt, data);
            texDrv->CopyFromMemory(pixbox, 0, 0);
        }
        
        _FreeImage(data);
    }

    size_t DiPixelBox::GetConsecutiveSize() const
    {
        return DiPixelBox::ComputeImageByteSize(GetWidth(), GetHeight(), format);
    }

    Demi::DiPixelBox DiPixelBox::GetSubVolume( const DiBox &def ) const
    {
        if(DiPixelBox::IsCompressedFormat(format))
        {
            if(def.left == left && def.top == top &&
                def.right == right && def.bottom == bottom)
            {
                return *this;
            }
            DI_WARNING("DiPixelBox::SubVolume() error");
        }
        if(!Contains(def))
        {
            DI_WARNING("DiPixelBox::SubVolume() error");
        }

        const size_t elemSize = GetNumElemBytes(format);
        DiPixelBox rval(def.GetWidth(), def.GetHeight(), format, 
            ((uint8*)data) + ((def.left-left)*elemSize)
            + ((def.top-top)*rowPitch*elemSize)
            );

        rval.rowPitch = rowPitch;
        rval.slicePitch = slicePitch;
        rval.format = format;

        return rval;
    }

    DiColor DiPixelBox::GetColourAt( size_t x, size_t y, size_t z )
    {
        DiColor cv;
        unsigned char pixelSize = (uint8)GetNumElemBytes(format);
        size_t pixelOffset = pixelSize * (z * slicePitch + y * rowPitch + x);
        UnpackColour(&cv, format, (unsigned char *)data + pixelOffset);
        return cv;
    }

    void DiPixelBox::SetColourAt( DiColor const &cv, size_t x, size_t y, size_t z )
    {
        unsigned char pixelSize = (uint8)GetNumElemBytes(format);
        size_t pixelOffset = pixelSize * (z * slicePitch + y * rowPitch + x);
        PackColour(cv, format, (unsigned char *)data + pixelOffset);
    }
    
    uint32 DiPixelBox::ComputeImageByteSize(uint32 mipmaps, uint32 surfaces, uint32 width, uint32 height,DiPixelFormat format)
    {
        uint32 size = 0;
        for(size_t mip=0; mip<=mipmaps; ++mip)
        {
            size += ComputeImageByteSize(width, height, format) * surfaces;
            if(width!=1) width /= 2;
            if(height!=1) height /= 2;
        }
        return size;
    }

    uint32 DiPixelBox::ComputeImageByteSize( uint32 width, uint32 height, DiPixelFormat format )
    {
        uint32 size = 0;
        uint32 numPixels = width * height ;
        switch(format)
        {
        case PF_DXT1:
            width   = ComputeDDSDimension(width);
            height  = ComputeDDSDimension(height);
            size    = ComputeImageByteSize(width, height, PF_A8R8G8B8) / 8;
            break;
        case PF_DXT3:
        case PF_DXT5:
            width   = ComputeDDSDimension(width);
            height  = ComputeDDSDimension(height);
            size    = ComputeImageByteSize(width, height, PF_A8R8G8B8) / 4;
            break;
        // Size calculations from the PVRTC OpenGL extension spec
        // http://www.khronos.org/registry/gles/extensions/IMG/IMG_texture_compression_pvrtc.txt
        // Basically, 32 bytes is the minimum texture size.  Smaller textures are padded up to 32 bytes
        case PF_PVRTC_RGB2:
        case PF_PVRTC_RGBA2:
        case PF_PVRTC2_2BPP:
            return (std::max((int)width, 16) * std::max((int)height, 8) * 2 + 7) / 8;
        case PF_PVRTC_RGB4:
        case PF_PVRTC_RGBA4:
        case PF_PVRTC2_4BPP:
            return (std::max((int)width, 8) * std::max((int)height, 8) * 4 + 7) / 8;
            
        case PF_ETC1_RGB8:
        case PF_ETC2_RGB8:
        case PF_ETC2_RGBA8:
        case PF_ETC2_RGB8A1:
            return ((width * height) >> 1);
        case PF_ATC_RGB:
            return ((width + 3) / 4) * ((height + 3) / 4) * 8;
        case PF_ATC_RGBA_EXPLICIT_ALPHA:
        case PF_ATC_RGBA_INTERPOLATED_ALPHA:
            return ((width + 3) / 4) * ((height + 3) / 4) * 16;

        default:
            size    = GetNumElemBytes(format) * numPixels;
        }
        DI_ASSERT(size);
        return size;
    }

    uint32 DiPixelBox::GetLevelDimension( uint32 dimension, uint32 level )
    {
        dimension >>=level;

        if(!dimension) 
            dimension=1;

        return dimension;
    }

    bool DiPixelBox::IsCompressedFormat( DiPixelFormat fmt )
    {
        const PixelFormatDescription &des = PixelFormatDescription::GetFormatDesc(fmt);
        return (des.flags & PFF_COMPRESSED) > 0;
    }

    uint32 DiPixelBox::GetFormatNumBlocks( uint32 dimension, DiPixelFormat fmt )
    {
        uint32 blockDimension = 0;
        if(IsCompressedFormat(fmt))
        {
            blockDimension = dimension / 4;
            if(dimension % 4) blockDimension++;
        }
        else
        {
            blockDimension = dimension;
        }
        return blockDimension;
    }

    uint32 DiPixelBox::GetFormatBlockSize( DiPixelFormat fmt )
    {
        return ComputeImageByteSize(1, 1, fmt);
    }
    static inline unsigned int FixedToFixed(uint32 value, unsigned int n, unsigned int p) 
    {
        if(n > p) 
        {
            value >>= n-p;
        } 
        else if(n < p)
        {
            if(value == 0)
                value = 0;
            else if(value == (static_cast<unsigned int>(1)<<n)-1)
                value = (1<<p)-1;
            else
                value = value*(1<<p)/((1<<n)-1);
        }
        return value;    
    }

    static inline unsigned int FloatToFixed(const float value, const unsigned int bits)
    {
        if(value <= 0.0f) return 0;
        else if (value >= 1.0f) return (1<<bits)-1;
        else return (unsigned int)(value * (1<<bits));     
    }

    static inline float FixedToFloat(unsigned value, unsigned int bits)
    {
        return (float)value/(float)((1<<bits)-1);
    }

    static inline unsigned int IntRead(const void *src, int n) 
    {
        switch(n) 
        {
        case 1:
            return ((uint8*)src)[0];
        case 2:
            return ((uint16*)src)[0];
        case 3:
            return ((uint32)((uint8*)src)[0])|
                ((uint32)((uint8*)src)[1]<<8)|
                ((uint32)((uint8*)src)[2]<<16);
        case 4:
            return ((uint32*)src)[0];
        } 
        return 0;
    }

    static inline void IntWrite(void *dest, const int n, const unsigned int value)
    {
        switch(n) 
        {
        case 1:
            ((uint8*)dest)[0] = (uint8)value;
            break;
        case 2:
            ((uint16*)dest)[0] = (uint16)value;
            break;
        case 3:
            ((uint8*)dest)[2] = (uint8)((value >> 16) & 0xFF);
            ((uint8*)dest)[1] = (uint8)((value >> 8) & 0xFF);
            ((uint8*)dest)[0] = (uint8)(value & 0xFF);
            break;
        case 4:
            ((uint32*)dest)[0] = (uint32)value;                
            break;                
        }        
    }

    void DiPixelBox::PackColour( const DiColor &colour, const DiPixelFormat pf, void* dest )
    {
        PackColour(colour.r, colour.g, colour.b, colour.a, pf, dest);
    }

    void DiPixelBox::PackColour( const uint8 r, const uint8 g, const uint8 b, const uint8 a, const DiPixelFormat pf, void* dest )
    {
        PixelFormatDescription des = PixelFormatDescription::GetFormatDesc(pf);
        unsigned int value = ((FixedToFixed(r, 8, des.rbits)<<des.rshift) & des.rmask) |
            ((FixedToFixed(g, 8, des.gbits)<<des.gshift) & des.gmask) |
            ((FixedToFixed(b, 8, des.bbits)<<des.bshift) & des.bmask) |
            ((FixedToFixed(a, 8, des.abits)<<des.ashift) & des.amask);
        IntWrite(dest, des.elemBytes, value);
    }

    void DiPixelBox::PackColour( const float r, const float g, const float b, const float a, const DiPixelFormat pf, void* dest )
    {
        PixelFormatDescription des = PixelFormatDescription::GetFormatDesc(pf);
        const unsigned int value = ((FloatToFixed(r, des.rbits)<<des.rshift) & des.rmask) |
            ((FloatToFixed(g, des.gbits)<<des.gshift) & des.gmask) |
            ((FloatToFixed(b, des.bbits)<<des.bshift) & des.bmask) |
            ((FloatToFixed(a, des.abits)<<des.ashift) & des.amask);
        IntWrite(dest, des.elemBytes, value);
    }

    void DiPixelBox::UnpackColour( DiColor *colour, DiPixelFormat pf, const void* src )
    {
        UnpackColour(&colour->r, &colour->g, &colour->b, &colour->a, pf, src);
    }

    void DiPixelBox::UnpackColour( uint8 *r, uint8 *g, uint8 *b, uint8 *a, DiPixelFormat pf, const void* src )
    {
        PixelFormatDescription des = PixelFormatDescription::GetFormatDesc(pf);
        const unsigned int value = IntRead(src, des.elemBytes);
        if(des.flags & PFF_LUMINANCE)
        {
            *r = *g = *b = (uint8)FixedToFixed(
                (value & des.rmask)>>des.rshift, des.rbits, 8);
        }
        else
        {
            *r = (uint8)FixedToFixed((value & des.rmask)>>des.rshift, des.rbits, 8);
            *g = (uint8)FixedToFixed((value & des.gmask)>>des.gshift, des.gbits, 8);
            *b = (uint8)FixedToFixed((value & des.bmask)>>des.bshift, des.bbits, 8);
        }
        if(des.flags & PFF_HASALPHA)
        {
            *a = (uint8)FixedToFixed((value & des.amask)>>des.ashift, des.abits, 8);
        }
        else
        {
            *a = 255; 
        }
    }

    void DiPixelBox::UnpackColour( float *r, float *g, float *b, float *a, DiPixelFormat pf, const void* src )
    {
        PixelFormatDescription des = PixelFormatDescription::GetFormatDesc(pf);
        const unsigned int value = IntRead(src, des.elemBytes);
        if(des.flags & PFF_LUMINANCE)
        {
            *r = *g = *b = FixedToFloat(
                (value & des.rmask)>>des.rshift, des.rbits);
        }
        else
        {
            *r = FixedToFloat((value & des.rmask)>>des.rshift, des.rbits);
            *g = FixedToFloat((value & des.gmask)>>des.gshift, des.gbits);
            *b = FixedToFloat((value & des.bmask)>>des.bshift, des.bbits);
        }
        if(des.flags & PFF_HASALPHA)
        {
            *a = FixedToFloat((value & des.amask)>>des.ashift, des.abits);
        }
        else
        {
            *a = 1.0f;
        }
    }

    DiPixelBox::DiPixelBox( const DiBox &extents, DiPixelFormat pixelFormat, void *pixelData/*=0*/ ) : DiBox(extents),
        data(pixelData), 
        format(pixelFormat)
    {
        SetConsecutive();
    }

    DiPixelBox::DiPixelBox(uint32 width, uint32 height, DiPixelFormat pixelFormat, void *pixelData/*=0*/) :
        DiBox(0, 0, width, height),
        data(pixelData), format(pixelFormat)
    {
        SetConsecutive();
    }

    uint32 DiPixelBox::GetNumElemBytes( DiPixelFormat format )
    {
        return PixelFormatDescription::GetFormatDesc(format).elemBytes;
    }

    void DiPixelBox::BulkPixelConversion( void *psrc, DiPixelFormat srcFormat, void *dest,
        DiPixelFormat dstFormat, unsigned int count )
    {
        DiPixelBox src(count, 1,srcFormat, psrc);
        DiPixelBox dst(count, 1,dstFormat, dest);

        BulkPixelConversion(src, dst);
    }

    void DiPixelBox::BulkPixelConversion( const DiPixelBox &src, const DiPixelBox &dst )
    {
        DI_ASSERT(src.GetWidth() == dst.GetWidth() &&
            src.GetHeight() == dst.GetHeight() );

        if(DiPixelBox::IsCompressedFormat(src.format) || DiPixelBox::IsCompressedFormat(dst.format))
        {
            if(src.format == dst.format)
            {
                memcpy(dst.data, src.data, src.GetConsecutiveSize());
                return;
            }
            else
            {
                DI_ASSERT("Pixel format doesn't match.");
            }
        }

        if(src.format == dst.format)
        {
            if(src.IsConsecutive() && dst.IsConsecutive())
            {
                memcpy(dst.data, src.data, src.GetConsecutiveSize());
                return;
            }

            const size_t srcPixelSize = DiPixelBox::GetNumElemBytes(src.format);
            const size_t dstPixelSize = DiPixelBox::GetNumElemBytes(dst.format);
            uint8 *srcptr = static_cast<uint8*>(src.data)
                + (src.left + src.top * src.rowPitch) * srcPixelSize;
            uint8 *dstptr = static_cast<uint8*>(dst.data)
                + (dst.left + dst.top * dst.rowPitch) * dstPixelSize;

            const size_t srcRowPitchBytes = src.rowPitch*srcPixelSize;
            const size_t srcSliceSkipBytes = src.GetSliceSkip()*srcPixelSize;

            const size_t dstRowPitchBytes = dst.rowPitch*dstPixelSize;
            const size_t dstSliceSkipBytes = dst.GetSliceSkip()*dstPixelSize;

            const size_t rowSize = src.GetWidth()*srcPixelSize;
            for(size_t y=src.top; y<src.bottom; y++)
            {
                memcpy(dstptr, srcptr, rowSize);
                srcptr += srcRowPitchBytes;
                dstptr += dstRowPitchBytes;
            }
            srcptr += srcSliceSkipBytes;
            dstptr += dstSliceSkipBytes;
            return;
        }

        const size_t srcPixelSize = DiPixelBox::GetNumElemBytes(src.format);
        const size_t dstPixelSize = DiPixelBox::GetNumElemBytes(dst.format);
        uint8 *srcptr = static_cast<uint8*>(src.data)
            + (src.left + src.top * src.rowPitch) * srcPixelSize;
        uint8 *dstptr = static_cast<uint8*>(dst.data)
            + (dst.left + dst.top * dst.rowPitch) * dstPixelSize;

        const size_t srcRowSkipBytes = src.GetRowSkip()*srcPixelSize;
        const size_t srcSliceSkipBytes = src.GetSliceSkip()*srcPixelSize;
        const size_t dstRowSkipBytes = dst.GetRowSkip()*dstPixelSize;
        const size_t dstSliceSkipBytes = dst.GetSliceSkip()*dstPixelSize;

        float r,g,b,a;
        for(size_t y=src.top; y<src.bottom; y++)
        {
            for(size_t x=src.left; x<src.right; x++)
            {
                UnpackColour(&r, &g, &b, &a, src.format, srcptr);
                PackColour(r, g, b, a, dst.format, dstptr);
                srcptr += srcPixelSize;
                dstptr += dstPixelSize;
            }
            srcptr += srcRowSkipBytes;
            dstptr += dstRowSkipBytes;
        }
        srcptr += srcSliceSkipBytes;
        dstptr += dstSliceSkipBytes;
    }
    
    uint32 DiPixelBox::GetMaxMipmaps(uint32 width, uint32 height)
    {
        uint32 count = 0;
        if((width > 0) && (height > 0))
        {
            do
            {
                if(width>1)
                    width = width/2;
                if(height>1)
                    height = height/2;
                
                count ++;
            } while(!(width == 1 && height == 1));
        }
        return count;
    }
    
    uint32 DiPixelBox::GetMaxMipmaps(uint32 width, uint32 height, uint32 depth)
    {
        uint32 count = 0;
        if((width > 0) && (height > 0) && (depth > 0))
        {
            do
            {
                if(width>1)
                    width = width/2;
                if(height>1)
                    height = height/2;
                if(depth>1)
                    depth = depth/2;
                
                count ++;
            } while(!(width == 1 && height == 1 && depth == 1));
        }
        return count;
    }
    
    uint32 DiPixelBox::GetNumComponents( DiPixelFormat format )
    {
        const PixelFormatDescription &des = PixelFormatDescription::GetFormatDesc(format);
        return des.componentCount;
    }

    void DiPixelBox::GetBitDepths(DiPixelFormat format, int rgba[4])
    {
        const PixelFormatDescription &des = PixelFormatDescription::GetFormatDesc(format);
        rgba[0] = des.rbits;
        rgba[1] = des.gbits;
        rgba[2] = des.bbits;
        rgba[3] = des.abits;
    }

    bool DiPixelBox::IsFloatingPoint(DiPixelFormat format)
    {
        const PixelFormatDescription &des = PixelFormatDescription::GetFormatDesc(format);
        return (des.flags & PFF_FLOAT) > 0;
    }

    PixelComponentType DiPixelBox::GetComponentType(DiPixelFormat format)
    {
        const PixelFormatDescription &des = PixelFormatDescription::GetFormatDesc(format);
        return des.componentType;
    }

    DiString DiPixelBox::GetPixelTypeName(DiPixelFormat eFormat)
    {
        static DiVector<DiString> names;
        if (names.empty())
        {
            names.resize(PIXEL_FORMAT_MAX);

            names[PF_R8G8B8] = "PF_R8G8B8";
            names[PF_A8R8G8B8] = "PF_A8R8G8B8";
            names[PF_A8B8G8R8] = "PF_A8B8G8R8";
            names[PF_X8R8G8B8] = "PF_X8R8G8B8";
            names[PF_A8] = "PF_A8";
            names[PF_L8] = "PF_L8";
            names[PF_A8L8] = "PF_A8L8";
            names[PF_DXT1] = "PF_DXT1";
            names[PF_DXT2] = "PF_DXT2";
            names[PF_DXT3] = "PF_DXT3";
            names[PF_DXT4] = "PF_DXT4";
            names[PF_DXT5] = "PF_DXT5";
            names[PF_R16F] = "PF_R16F";
            names[PF_G16R16F] = "PF_G16R16F";
            names[PF_A16B16G16R16F] = "PF_A16B16G16R16F";
            names[PF_R32F] = "PF_R32F";
            names[PF_G32R32F] = "PF_G32R32F";
            names[PF_A32B32G32R32F] = "PF_A32B32G32R32F";
            names[PF_PVRTC_RGB2] = "PF_PVRTC_RGB2";
            names[PF_PVRTC_RGBA2] = "PF_PVRTC_RGBA2";
            names[PF_PVRTC_RGB4] = "PF_PVRTC_RGB4";
            names[PF_PVRTC_RGBA4] = "PF_PVRTC_RGBA4";
            names[PF_PVRTC2_2BPP] = "PF_PVRTC2_2BPP";
            names[PF_PVRTC2_4BPP] = "PF_PVRTC2_4BPP";
            names[PF_ETC1_RGB8] = "PF_ETC1_RGB8";
            names[PF_ETC2_RGB8] = "PF_ETC2_RGB8";
            names[PF_ETC2_RGBA8] = "PF_ETC2_RGBA8";
            names[PF_ETC2_RGB8A1] = "PF_ETC2_RGB8A1";
            names[PF_ATC_RGB] = "PF_ATC_RGB";
            names[PF_ATC_RGBA_EXPLICIT_ALPHA] = "PF_ATC_RGBA_EXPLICIT_ALPHA";
            names[PF_ATC_RGBA_INTERPOLATED_ALPHA] = "PF_ATC_RGBA_INTERPOLATED_ALPHA";
            names[PF_DEPTH] = "PF_DEPTH";
        }

        return names[eFormat];
    }

    DiMap<DiPixelFormat,PixelFormatDescription> PixelFormatDescription::s_kDescs;

    PixelFormatDescription PixelFormatDescription::GetFormatDesc( DiPixelFormat format )
    {
        DiMap<DiPixelFormat,PixelFormatDescription>::iterator it = s_kDescs.find(format);
        if (it != s_kDescs.end())
        {
            return it->second;
        }

        switch(format)
        {
        case PF_R8G8B8:
            {
                PixelFormatDescription desc =
                {
                    3,  
                    PFF_NATIVEENDIAN,
                    PCT_BYTE, 3,
                    8, 8, 8, 0,
                    0xFF0000, 0x00FF00, 0x0000FF, 0,
                    16, 8, 0, 0
                };
                s_kDescs[format] = desc;
                return desc;
            }
            break;
        case PF_A8R8G8B8:
            {
                PixelFormatDescription desc = 
                {
                    4,
                    PFF_HASALPHA | PFF_NATIVEENDIAN,
                    PCT_BYTE, 4,
                    8, 8, 8, 8,
                    0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000,
                    16, 8, 0, 24
                };
                s_kDescs[format] = desc;
                return desc;
            }
            break;
        case PF_A8B8G8R8:
            {
                PixelFormatDescription desc = 
                {
                    4,
                    PFF_HASALPHA | PFF_NATIVEENDIAN,
                    PCT_BYTE, 4,
                    8, 8, 8, 8,
                    0x000000FF, 0x0000FF00, 0x00FF0000, 0xFF000000,
                    0, 8, 16, 24,
                };
                s_kDescs[format] = desc;
                return desc;
            }
            break;
        case PF_X8R8G8B8:
            {
                PixelFormatDescription desc = 
                {
                    4,
                    PFF_NATIVEENDIAN,
                    PCT_BYTE, 3,
                    8, 8, 8, 0,
                    0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000,
                    16, 8, 0, 24
                };
                s_kDescs[format] = desc;
                return desc;
            }
            break;
        case PF_A8:
            {
                PixelFormatDescription desc = 
                {
                    1,
                    PFF_HASALPHA | PFF_NATIVEENDIAN,
                    PCT_BYTE, 1,
                    0, 0, 0, 8,
                    0, 0, 0, 0xFF, 0, 0, 0, 0
                };
                s_kDescs[format] = desc;
                return desc;
            }
            break;
        case PF_A8L8:
            {
                PixelFormatDescription desc =
                {
                    2,
                    PFF_HASALPHA | PFF_LUMINANCE,
                    PCT_BYTE, 2,
                    8, 0, 0, 8,
                    0, 0, 0, 0, 0, 0, 0, 0
                };
                s_kDescs[format] = desc;
                return desc;
            }
                break;
        case PF_L8:
            {
                PixelFormatDescription desc = 
                {
                    1,
                    PFF_LUMINANCE | PFF_NATIVEENDIAN,
                    PCT_BYTE, 1,
                    8, 0, 0, 0,
                    0xFF, 0, 0, 0, 0, 0, 0, 0
                };
                s_kDescs[format] = desc;
                return desc;
            }
            break;
        case PF_R32F:
            {
                PixelFormatDescription desc = 
                {
                    4,
                    PFF_FLOAT,
                    PCT_FLOAT32, 1,
                    32, 0, 0, 0,
                    0, 0, 0, 0, 0, 0, 0, 0
                };
                s_kDescs[format] = desc;
                return desc;
            }
            break;
        case PF_R16F:
            {
                PixelFormatDescription desc =
                {
                    2,
                    PFF_FLOAT,
                    PCT_FLOAT16, 1,
                    16, 0, 0, 0,
                    0, 0, 0, 0, 0, 0, 0, 0
                };
                s_kDescs[format] = desc;
                return desc;
            }
            break;
        case PF_G16R16F:
            {
                PixelFormatDescription desc =
                {
                    4,
                    PFF_FLOAT,
                    PCT_FLOAT16, 2,
                    16, 16, 0, 0,
                    0, 0, 0, 0, 0, 0, 0, 0
                };
                s_kDescs[format] = desc;
                return desc;
            }
            break;
        case PF_A16B16G16R16F:
            {
                PixelFormatDescription desc =
                {
                    8,
                    PFF_FLOAT | PFF_HASALPHA,
                    PCT_FLOAT16, 4,
                    16, 16, 16, 16,
                    0, 0, 0, 0, 0, 0, 0, 0
                };
                s_kDescs[format] = desc;
                return desc;
            }
            break;
        case PF_G32R32F:
            {
                PixelFormatDescription desc =
                {
                    8,
                    PFF_FLOAT,
                    PCT_FLOAT32, 2,
                    32, 32, 0, 0,
                    0, 0, 0, 0, 0, 0, 0, 0
                };
                s_kDescs[format] = desc;
                return desc;
            }
            break;
        case PF_A32B32G32R32F:
            {
                PixelFormatDescription desc =
                {
                    16,
                    PFF_FLOAT | PFF_HASALPHA,
                    PCT_FLOAT32, 4,
                    32, 32, 32, 32,
                    0, 0, 0, 0, 0, 0, 0, 0
                };
                s_kDescs[format] = desc;
                return desc;
            }
            break;
        case PF_DXT1:
            {
                PixelFormatDescription desc = 
                {
                    0,
                    PFF_COMPRESSED | PFF_HASALPHA,
                    PCT_BYTE, 3, 
                    0, 0, 0, 0,
                    0, 0, 0, 0, 0, 0, 0, 0
                };
                s_kDescs[format] = desc;
                return desc;
            }
            break;
        case PF_DXT2:
            {
                PixelFormatDescription desc = 
                {
                    0,
                    PFF_COMPRESSED | PFF_HASALPHA,
                    PCT_BYTE, 4,
                    0, 0, 0, 0,
                    0, 0, 0, 0, 0, 0, 0, 0
                };
                s_kDescs[format] = desc;
                return desc;
            }
            break;
        case PF_DXT3:
            {
                PixelFormatDescription desc = 
                {
                    0,
                    PFF_COMPRESSED | PFF_HASALPHA,
                    PCT_BYTE, 4,
                    0, 0, 0, 0,
                    0, 0, 0, 0, 0, 0, 0, 0
                };
                s_kDescs[format] = desc;
                return desc;
            }
            break;
        case PF_DXT4:
            {
                PixelFormatDescription desc = 
                {
                    0,
                    PFF_COMPRESSED | PFF_HASALPHA,
                    PCT_BYTE, 4,
                    0, 0, 0, 0,
                    0, 0, 0, 0, 0, 0, 0, 0
                };
                s_kDescs[format] = desc;
                return desc;
            }
            break;
        case PF_DXT5:
            {
                PixelFormatDescription desc = 
                {
                    0,
                    PFF_COMPRESSED | PFF_HASALPHA,
                    PCT_BYTE, 4,
                    0, 0, 0, 0,
                    0, 0, 0, 0, 0, 0, 0, 0
                };
                s_kDescs[format] = desc;
                return desc;
            }
        case PF_DEPTH:
            {
                PixelFormatDescription desc =
                {
                    4,
                    PFF_DEPTH,
                    PCT_FLOAT16, 1,
                    16, 0, 0, 0,
                    0, 0, 0, 0, 0, 0, 0, 0
                };
                s_kDescs[format] = desc;
                return desc;
            }
        case PF_PVRTC_RGB2:
            {
                PixelFormatDescription desc =
                {
                    0,
                    PFF_COMPRESSED,
                    PCT_BYTE, 3,
                    0, 0, 0, 0,
                    0, 0, 0, 0, 0, 0, 0, 0
                };
                s_kDescs[format] = desc;
                return desc;
            }
        case PF_PVRTC_RGBA2:
            {
                PixelFormatDescription desc =
                {
                    0,
                    PFF_COMPRESSED | PFF_HASALPHA,
                    PCT_BYTE, 4,
                    0, 0, 0, 0,
                    0, 0, 0, 0, 0, 0, 0, 0
                };
                s_kDescs[format] = desc;
                return desc;
            }
        case PF_PVRTC_RGB4:
            {
                PixelFormatDescription desc =
                {
                    0,
                    PFF_COMPRESSED,
                    PCT_BYTE, 3,
                    0, 0, 0, 0,
                    0, 0, 0, 0, 0, 0, 0, 0
                };
                s_kDescs[format] = desc;
                return desc;
            }
        case PF_PVRTC_RGBA4:
            {
                PixelFormatDescription desc =
                {
                    0,
                    PFF_COMPRESSED | PFF_HASALPHA,
                    PCT_BYTE, 4,
                    0, 0, 0, 0,
                    0, 0, 0, 0, 0, 0, 0, 0
                };
                s_kDescs[format] = desc;
                return desc;
            }
        case PF_PVRTC2_2BPP:
            {
                PixelFormatDescription desc =
                {
                    0,
                    PFF_COMPRESSED | PFF_HASALPHA,
                    PCT_BYTE, 4,
                    0, 0, 0, 0,
                    0, 0, 0, 0, 0, 0, 0, 0
                };
                s_kDescs[format] = desc;
                return desc;
            }
        case PF_PVRTC2_4BPP:
            {
                PixelFormatDescription desc =
                {
                    0,
                    PFF_COMPRESSED | PFF_HASALPHA,
                    PCT_BYTE, 4,
                    0, 0, 0, 0,
                    0, 0, 0, 0, 0, 0, 0, 0
                };
                s_kDescs[format] = desc;
                return desc;
            }
        case PF_ETC1_RGB8:
            {
                PixelFormatDescription desc =
                {
                    0,
                    PFF_COMPRESSED,
                    PCT_BYTE, 3,
                    0, 0, 0, 0,
                    0, 0, 0, 0, 0, 0, 0, 0
                };
                s_kDescs[format] = desc;
                return desc;
            }
        case PF_ETC2_RGB8:
            {
                PixelFormatDescription desc =
                {
                    0,
                    PFF_COMPRESSED,
                    PCT_BYTE, 3,
                    0, 0, 0, 0,
                    0, 0, 0, 0, 0, 0, 0, 0
                };
                s_kDescs[format] = desc;
                return desc;
            }
        case PF_ETC2_RGBA8:
            {
                PixelFormatDescription desc =
                {
                    0,
                    PFF_COMPRESSED | PFF_HASALPHA,
                    PCT_BYTE, 4,
                    0, 0, 0, 0,
                    0, 0, 0, 0, 0, 0, 0, 0
                };
                s_kDescs[format] = desc;
                return desc;
            }
        case PF_ETC2_RGB8A1:
            {
                PixelFormatDescription desc =
                {
                    0,
                    PFF_COMPRESSED | PFF_HASALPHA,
                    PCT_BYTE, 4,
                    0, 0, 0, 0,
                    0, 0, 0, 0, 0, 0, 0, 0
                };
                s_kDescs[format] = desc;
                return desc;
            }
        case PF_ATC_RGB:
            {
                PixelFormatDescription desc =
                {
                    0,
                    PFF_COMPRESSED,
                    PCT_BYTE, 3,
                    0, 0, 0, 0,
                    0, 0, 0, 0, 0, 0, 0, 0
                };
                s_kDescs[format] = desc;
                return desc;
            }
        case PF_ATC_RGBA_EXPLICIT_ALPHA:
            {
                PixelFormatDescription desc =
                {
                    0,
                    PFF_COMPRESSED | PFF_HASALPHA,
                    PCT_BYTE, 4,
                    0, 0, 0, 0,
                    0, 0, 0, 0, 0, 0, 0, 0
                };
                s_kDescs[format] = desc;
                return desc;
            }
        case PF_ATC_RGBA_INTERPOLATED_ALPHA:
            {
                PixelFormatDescription desc =
                {
                    0,
                    PFF_COMPRESSED | PFF_HASALPHA,
                    PCT_BYTE, 4,
                    0, 0, 0, 0,
                    0, 0, 0, 0, 0, 0, 0, 0
                };
                s_kDescs[format] = desc;
                return desc;
            }
        default:
            DI_WARNING("Unsupported pixel format: %d", format);
            break;
        }
        return PixelFormatDescription();
    }
}
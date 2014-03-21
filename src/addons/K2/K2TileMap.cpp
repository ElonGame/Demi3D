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

#include "K2Pch.h"
#include "K2TileMap.h"

namespace Demi
{
    DiK2TileMap::DiK2TileMap()
        :mWidth(0)
        ,mHeight(0)
    {
        mBuffer[0] = nullptr;
        mBuffer[1] = nullptr;
    }
    
    DiK2TileMap::~DiK2TileMap()
    {
        Unload();
    }
    
    void DiK2TileMap::Load(DiDataStreamPtr data)
    {
        int w = 0, h = 0;
        int layers = 0;
        
        data->Read(&layers, sizeof(int));
        data->Read(&w, sizeof(int));
        data->Read(&h, sizeof(int));
        
        bool asFloat = true;
        if(w < 0)
        {
            asFloat = false;
            w = -w;
        }
        
        mWidth = w;
        mHeight = h;
        
        Unload();
        
        for (int l = 0; l < 2; l++)
        {
            mBuffer[l] = DI_NEW K2TileLayer[w * h];
            
            for (int i = 0; i < w*h; ++i)
            {
                if (asFloat)
                {
                    int t;
                    data->Read(&t, sizeof(int));
                    mBuffer[l][i].data0 = (uint16)t;
                    data->Read(&t, sizeof(int));
                    mBuffer[l][i].data1 = (uint16)t;
                    data->Read(&t, sizeof(int));
                    mBuffer[l][i].textureID = (uint16)t;
                    data->Skip(sizeof(float)*7); // I don't know what they are doing
                }
                else
                {
                    data->Read(&mBuffer[l][i], sizeof(K2TileLayer));
                    data->Skip(sizeof(float)*3 + sizeof(uint16)); // I don't know either
                }
            }
        }
    }
    
    void DiK2TileMap::Unload()
    {
        if(mBuffer)
        {
            DI_DELETE[] mBuffer[0];
            DI_DELETE[] mBuffer[1];
            mBuffer[0] = nullptr;
            mBuffer[1] = nullptr;
        }
    }
}
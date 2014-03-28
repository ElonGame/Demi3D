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
#include "K2Entity.h"
#include "K2Property.h"

namespace Demi
{
    DiK2Entity::DiK2Entity()
        :mID(INVALID_OBJ_ID)
    {

    }

    DiK2Entity::~DiK2Entity()
    {
        auto it = mPropertyMap.begin();
        auto itEnd = mPropertyMap.end();
        while (it != itEnd)
        {
            it->second->Deactivate();
            DI_DELETE(it->second);
            it++;
        }
        mPropertyMap.clear();
    }

    void DiK2Entity::Update(float dt)
    {
        auto it = mPropertyMap.begin();
        auto itEnd = mPropertyMap.end();
        while (it != itEnd)
        {
            it->second->Update(dt);
            it++;
        }
    }
}
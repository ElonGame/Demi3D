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
#include "K2DynEntity.h"

namespace Demi
{
    DiK2DynEntity::DiK2DynEntity()
        : mMoveProperty(nullptr)
    {

    }

    DiK2DynEntity::~DiK2DynEntity()
    {

    }

    void DiK2DynEntity::InitComponents()
    {
        DiK2GameEntity::InitComponents();
        mMoveProperty = createProperty<DiK2MoveProperty>();
    }

}
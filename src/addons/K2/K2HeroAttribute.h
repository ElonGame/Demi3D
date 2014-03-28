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

#ifndef DiK2HeroAttribute_h__
#define DiK2HeroAttribute_h__

#include "K2Prerequisites.h"
#include "K2Attribute.h"
#include "K2AttrStructure.h"

namespace Demi
{
    class DEMI_K2_API DiK2HeroAttr : public DiK2Attribute
    {
    public:

        DiK2HeroAttr();

        virtual ~DiK2HeroAttr();

    public:

        virtual const K2ObjID   GetID() const;

        virtual const int       GetHP() const;

        virtual const int       GetMaxHP() const;

        virtual const int       GetDirection() const;

        virtual const float     GetMoveSpeed() const;

        virtual void            Update(float dt) {}

    protected:

        K2HeroAttributes        mHeroAttrs;
    };
}

#endif

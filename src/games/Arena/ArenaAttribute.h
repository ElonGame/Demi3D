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

#ifndef ArenaAttribute_h__
#define ArenaAttribute_h__

#include "ArenaPrerequisites.h"

namespace Demi
{
    /** Attribute interface for entities
     */
    class ArAttribute
    {
    public:

        ArAttribute(ArObjID id) : mID(id) {}

    public:

        virtual ArObjID   GetID() const { return mID; }

        virtual void      Update(float dt) {}

        virtual void      LoadAttribute(const DiXMLElement& node) = 0;

        virtual void      LoadAttributeConfig(const DiString& configFile);

    private:

        ArObjID mID { INVALID_OBJ_ID };
    };
}

#endif

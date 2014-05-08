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

#ifndef ArenaPrerequisites_h__
#define ArenaPrerequisites_h__

#include "K2Prerequisites.h"

namespace Demi
{
    class ArHeroEntity;
    class ArNPCEntity;
    class ArGame;
    class ArGameCamera;
    class ArInput;
    class ArGameApp;
    class ArEntity;
    class ArEntityManager;
    class ArGameEntity;
    class ArProperty;
    class ArAttribute;
    class ArLevel;
    class ArAICommand;

    typedef shared_ptr<ArGameEntity>  ArGameEntityPtr;
    typedef shared_ptr<ArHeroEntity>  ArHeroEntityPtr;
}

#include "ArenaStateDefines.h"
#include "ArenaGameDefines.h"

#endif

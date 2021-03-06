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

#ifndef DebugAssert_h__
#define DebugAssert_h__

#include "PlatformFunction.h"

#ifdef _DEMI_RELEASE
#   define DI_ASSERT(condition)
#   define DI_ASSERT_MESSAGE(condition,message)
#else
#   if DEMI_COMPILER == DEMI_COMPILER_MSVC
#       define DI_ASSERT(condition) DI_ASSERT_MESSAGE(condition,NULL)
#       define DI_ASSERT_MESSAGE(condition,message)                                  \
        {                                                                            \
            static bool ignoreAssert = false;                                        \
            if(!ignoreAssert && !(condition))                                        \
            {                                                                        \
                if(Demi::DiPlatformFunc::AssertDlg(#condition,__FILE__,__LINE__,ignoreAssert))    \
                {                    \
                    __asm int 3      \
                }                    \
            }                        \
        }
#   else
#       define DI_ASSERT(condition) assert(condition)
#       define DI_ASSERT_MESSAGE(condition,message) assert(condition && message)
#   endif
#endif

#define DI_ASSERT_FAIL DI_ASSERT(false)

#endif
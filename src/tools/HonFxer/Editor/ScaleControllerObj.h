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

#ifndef ScaleControllerObject_h__
#define ScaleControllerObject_h__

#include "K2Prerequisites.h"
#include "FxerPrerequisites.h"
#include "BaseEditorObject.h"
#include "ControllerBaseObj.h"
#include "PropertyTypes.h"

namespace Demi 
{
    class DiScaleControllerObj final : public DiControllerBaseObj
    {
    public:

        DiScaleControllerObj();

        ~DiScaleControllerObj();

    public:

        DiString    GetType() override { return "ScaleController"; }

        DiString    GetControllerType() { return "Scale"; }

        DiString    GetUICaption() override;

        DiString    GetUINodeType() { return "File"; }

    public:

        DiScaleController*  GetController();

    protected:

        void        InitPropertyTable() override;
    };
}

#endif

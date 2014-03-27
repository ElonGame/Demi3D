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
#include "K2AnimatedObj.h"
#include "K2Clip.h"
#include "K2Model.h"
#include "CullNode.h"
#include "GfxDriver.h"
#include "SceneManager.h"

namespace Demi
{
    DiK2AnimatedObj::DiK2AnimatedObj()
    {
    }

    DiK2AnimatedObj::~DiK2AnimatedObj()
    {
    }

    DiK2ModelPtr DiK2AnimatedObj::LoadModel(const DiString& mdf)
    {
        DiSceneManager* sm = Driver->GetSceneManager();
        mNode = sm->GetRootNode()->CreateChild();

        mModel = make_shared<DiK2Model>(mdf);
        mModel->GetAnimation()->Play(K2_DEFAULT_ANIM);
        mNode->AttachObject(mModel);

        return mModel;
    }

}
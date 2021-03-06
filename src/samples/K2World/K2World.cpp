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

#include "Demi.h"
#include "DemoFrame.h"
#include "DebugHelper.h"

#include "K2Model.h"
#include "K2Clip.h"
#include "EnginePlugin.h"
#include "K2World.h"
#include "K2Configs.h"
#include "K2Terrain.h"
#include "K2Plugin.h"

DiK2World* world = nullptr;

void InitScene()
{
    DI_INSTALL_PLUGIN(DiK2);
    
    DiBase::Driver->GetMainRenderWindow()->GetSceneCanvas()->SetClearColor(DiColor(0.5f,0.5f,0.5f));

    world = DI_NEW DiK2World();
    //world->Load("sm_0.s2z");
    world->Load("midwars.s2z");

    DiVec2 worldsize = world->mTerrain->GetWorldSize();

    auto cameraHelper = DemiDemo::GetApp()->GetCameraHelper();
    cameraHelper->SetStyle(CS_FREELOOK);
    cameraHelper->SetTopSpeed(300);
    cameraHelper->GetCamera()->SetPosition(worldsize.x / 2, 500, worldsize.y / 2);
    //cameraHelper->GetCamera()->SetPosition(3104.90723f, 1610.28467f, 4649.14502f);
    cameraHelper->GetCamera()->SetOrientation(DiQuat(0.308038354f,-0.225031450f,-0.746425748,-0.545306683f));
    
    //cameraHelper->GetCamera()->SetPosition(11720.666f, 1072.60095f, 4327.33447f);
    //cameraHelper->GetCamera()->SetPosition(12518.6621f, 634.203735f, 3195.90112f);
    //cameraHelper->GetCamera()->SetOrientation(DiQuat(-0.383136421f,0.168790638f,-0.831088602f,-0.366114467f));
    //cameraHelper->Enable(false);

    DiPostEffectManager* peMgr = DiBase::Driver->GetMainRenderWindow()->GetPostEffectManager();
    DiPostEffect* bloom = peMgr->GetEffect("Bloom");
    if (bloom)
        bloom->SetEnable(true);
}

void UpdateScene()
{
    auto cam = DiBase::Driver->GetSceneManager()->GetCamera();
    auto pos = cam->GetPosition();
    auto rot = cam->GetOrientation();
}

int main(int argc, char *argv[])
{
    DemiDemo app( DemoConfig("Demi Sample: Hon World",640,1136));
	app.SetInitCallback(InitScene);
	app.SetUpdateCallback(UpdateScene);
    app.SetShutdownCallback([&](){
        DI_UNINSTALL_PLUGIN(DiK2);
    });
	app.Open(argc, argv);
    
	return 0;
}
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

void AddMeshes()
{
    DiSceneManager* sm = DiBase::Driver->GetSceneManager();
	
	DiCullNode* cullnode = sm->GetRootNode()->CreateChild();
	DiModelPtr model = make_shared<DiModel>("car","F50N_H.model");

	cullnode->AttachObject(model);
	cullnode->SetScale(0.02f,0.02f,0.02f);
}

void InitScene()
{
	DiSceneManager* sm = DiBase::Driver->GetSceneManager();

	sm->SetAmbientColor(DiColor(0.02f,0.02f,0.02f));

    DiDirLightPtr dirlight0 = make_shared<DiDirLight>();
	dirlight0->SetColor(DiColor(1,1,1,2));
	dirlight0->SetDirection(DiVec3(2,1.2f,10).normalisedCopy());

    DiDirLightPtr dirlight1 = make_shared<DiDirLight>();
	dirlight1->SetColor(DiColor(1,1,1,1));
	dirlight1->SetDirection(DiVec3(-2,-1.2f,-10).normalisedCopy());

    sm->AttachObject(dirlight0);
    sm->AttachObject(dirlight1);
	sm->GetSkybox()->Enable(10000,"bridge.dds");

	AddMeshes();
}

int main(int argc, char *argv[])
{
    DemiDemo app( DemoConfig("Demi3D Sample - Car", 1500, 800));
	app.SetInitCallback(InitScene);
	app.Open();

	return 0;
}
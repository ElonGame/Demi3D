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

const int numMeshes = 3;
DiCullNode* lightSphereNode = nullptr;

void AddMeshes()
{
    DiSceneManager* sm = DiBase::Driver->GetSceneManager();

	DiMaterialPtr m0 = DiMaterial::QuickCreate("lambert_v","lambert_p",SHADER_FLAG_USE_ENV_MAP | SHADER_FLAG_ENV_MAP_MIX);
    m0->SetDiffuse(DiColor(1, 0, 0));
    m0->SetAmbient(DiColor(0.6f, 0.2f, 0));
	m0->GetShaderParameter()->WriteFloat("reflectivity",0.3f);
	m0->GetShaderParameter()->WriteTextureCUBE("envMap","SwedishRoyalCastle.dds");

	DiMaterialPtr m1 = DiMaterial::QuickCreate("lambert_v","lambert_p",SHADER_FLAG_USE_ENV_MAP | SHADER_FLAG_ENV_MAP_REFRACT);
    m1->SetDiffuse(DiColor(0, 1, 0));
    m1->SetAmbient(DiColor(0.6f, 0.4f, 0));
	m1->GetShaderParameter()->WriteFloat("reflectivity",1.0f);
	m1->GetShaderParameter()->WriteFloat("refractionRatio",0.95f);
	m1->GetShaderParameter()->WriteTextureCUBE("envMap","SwedishRoyalCastle.dds");

	DiMaterialPtr m2 = DiMaterial::QuickCreate("lambert_v","lambert_p",SHADER_FLAG_USE_ENV_MAP | SHADER_FLAG_ENV_MAP_MUL);
    m2->SetDiffuse(DiColor(0, 0, 1));
    m2->SetAmbient(DiColor(0.67f, 0.67f, 0.67f));
	m2->GetShaderParameter()->WriteFloat("reflectivity",1.0f);
	m2->GetShaderParameter()->WriteTextureCUBE("envMap","SwedishRoyalCastle.dds");

	DiVector<DiMaterialPtr> materials;
	materials.push_back(m1);
	materials.push_back(m2);
	materials.push_back(m0);

	for(int i = 0; i < numMeshes; i++)
    {
		DiCullNode* cullnode = sm->GetRootNode()->CreateChild();
		
        DiModelPtr model = make_shared<DiModel>("whead", "WaltHead.model");
		model->SetMaterial(materials[i]);
		cullnode->AttachObject(model);
		cullnode->Rotate(DiVec3::UNIT_Y, DiRadian(DiMath::PI));
        cullnode->SetPosition((i - 1)*70.0f, 0, 10);
	}
}

void InitScene()
{
    DiSceneManager* sm = DiBase::Driver->GetSceneManager();
    sm->SetAmbientColor(DiColor(1, 1, 1, 1));

	// visible mesh for the point light
    DiPointLightPtr pointlight = make_shared<DiPointLight>();
    pointlight->SetColor(DiColor(1, 1, 1, 2));
	pointlight->SetAttenuation(200);

	DiSimpleShapePtr lightSphere = make_shared<DiSimpleShape>("lightsphere");
    lightSphere->CreateSphere(1, 16, 8);
	DiMaterialPtr m = DiMaterial::QuickCreate("basic_v","basic_p");
    m->SetDiffuse(DiColor(1, 0.8f, 0));
	lightSphere->SetMaterial(m);

	lightSphereNode = sm->GetRootNode()->CreateChild();
	lightSphereNode->AttachObject(lightSphere);
	lightSphereNode->AttachObject(pointlight);

    sm->GetSkybox()->Enable(1000, "SwedishRoyalCastle.dds");

    DemiDemo::GetApp()->GetCameraHelper()->SetStyle(CS_SMOOTH);
    sm->GetCamera()->SetPosition(0, 0, 200);
    sm->GetCamera()->LookAt(0, 0, 0);

	AddMeshes();
}

void UpdateScene()
{
	// rotating point light
	float time = DiBase::Driver->GetElapsedSecond() / 2;
	lightSphereNode->SetPosition(130 * DiMath::Cos(time), 30 * DiMath::Sin(time), 70 * DiMath::Sin(time));
}

int main(int argc, char *argv[])
{
    DemiDemo app( DemoConfig("Demi3D Sample - Environment Map", 1500, 800));
	app.SetInitCallback(InitScene);
	app.SetUpdateCallback(UpdateScene);
	app.Open(argc, argv);

	return 0;
}
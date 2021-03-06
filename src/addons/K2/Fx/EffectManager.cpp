
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

/****************************************************
  This module was originally from Particle Universe
  Repo: https://github.com/scrawl/particleuniverse
  License: MIT 
****************************************************/

#include "K2Pch.h"
#include "EffectManager.h"
#include "ParticleElement.h"
#include "ParticleEmitter.h"
#include "ParticleController.h"
#include "ParticleSystem.h"
#include "ParticleEmitterFactory.h"
#include "ParticleControllerFactory.h"
#include "ParticleRendererFactory.h"

#include "BillboardRender.h"

#include "PointEmitter.h"
#include "LineEmitter.h"
#include "BoxEmitter.h"
#include "CircleEmitter.h"

#include "BoxColliderController.h"
#include "ColorController.h"
#include "GeometryRotatorController.h"
#include "GravityController.h"
#include "JetController.h"
#include "LinearForceController.h"
#include "PlaneColliderController.h"
#include "RandomiserController.h"
#include "ScaleController.h"
#include "SineForceController.h"
#include "SphereColliderController.h"
#include "VortexController.h"
#include "TextureRotatorController.h"

#include "SceneManager.h"
#include "RenderWindow.h"
#include "AssetManager.h"
#include "TokensParser.h"

_IMPLEMENT_SINGLETON(Demi::DiEffectManager);

namespace Demi
{
    DiEffectManager::DiEffectManager () :
        mLastCreatedTemplateName(DiString::BLANK)
    {
        InitFactories();
        
        LoadEffects();
    }
    
    DiEffectManager::~DiEffectManager ()
    {
        DestroyAllParticleSystemTemplates();

        ReleaseFactories();
    }

    void DiEffectManager::DestroyAllParticleSystemTemplates() 
    {
        mParticleSystemTemplates.clear();
    }
    
    void DiEffectManager::AddEmitterFactory(DiParticleEmitterFactory* factory)
    {
        DiString type = factory->GetEmitterType();
        mEmitterFactories[type] = factory;
    }
    
    DiParticleEmitterFactory* DiEffectManager::GetEmitterFactory(const DiString& emitterType)
    {
        EmitterFactoryMap::iterator it = mEmitterFactories.find(emitterType);
        if (it != mEmitterFactories.end())
        {
            return it->second;
        }
        return 0;
    }
    
    void DiEffectManager::RemoveEmitterFactory(const DiString& emitterType)
    {
        auto it = mEmitterFactories.find(emitterType);
        if (it == mEmitterFactories.end())
        {
            DI_WARNING("Cannot find the emitter type: %s", emitterType.c_str());
            return;
        }

        mEmitterFactories.erase(it);
    }
    
    void DiEffectManager::DestroyEmitterFactory(const DiString& emitterType)
    {
        auto it = mEmitterFactories.find(emitterType);
        if (it == mEmitterFactories.end())
        {
            DI_WARNING("Cannot find the emitter type: %s", emitterType.c_str());
            return;
        }

        SAFE_DELETE(it->second);
        mEmitterFactories.erase(it);
    }
    
    DiParticleEmitter* DiEffectManager::CreateEmitter(const DiString& emitterType)
    {
        auto it = mEmitterFactories.find(emitterType);

        if (it == mEmitterFactories.end())
        {
            DI_WARNING("Cannot find the emitter type: %s", emitterType.c_str());
            return nullptr;
        }

        return it->second->CreateEmitter();
    }
    
    DiParticleEmitter* DiEffectManager::CloneEmitter(DiParticleEmitter* emitter)
    {
        if (!emitter)
        {
            return 0;
        }

        DiParticleEmitter* clonedEmitter = CreateEmitter(emitter->GetEmitterType());
        emitter->CopyTo(clonedEmitter);
        return clonedEmitter;
    }
    
    DiParticleElement* DiEffectManager::CreateElement(void)
    {
        DiParticleElement* technique = DI_NEW DiParticleElement();
        technique->SetRenderer(CreateRenderer("Billboard"));
        return technique;
    }
    
    DiParticleElement* DiEffectManager::CloneElement(DiParticleElement* element)
    {
        if (!element)
        {
            return 0;
        }

        DiParticleElement* clonedTechnique = CreateElement();
        element->CopyTo(clonedTechnique);
        return clonedTechnique;
    }
    
    void DiEffectManager::AddControllerFactory(DiParticleControllerFactory* factory)
    {
        DiString type = factory->GetControllerType();
        mControllerFactories[type] = factory;
    }
    
    DiParticleControllerFactory* DiEffectManager::GetControllerFactory(const DiString& affectorType)
    {
        auto it = mControllerFactories.find(affectorType);
        if (it != mControllerFactories.end())
        {
            return it->second;
        }
        return 0;
    }
    
    void DiEffectManager::RemoveControllerFactory(const DiString& controllerType)
    {
        auto it = mControllerFactories.find(controllerType);
        if (it == mControllerFactories.end())
        {
            DI_WARNING("Cannot find the controller type: %s", controllerType.c_str());
            return;
        }

        mControllerFactories.erase(it);
    }
    
    void DiEffectManager::DestroyControllerFactory(const DiString& controllerType)
    {
        auto it = mControllerFactories.find(controllerType);
        if (it == mControllerFactories.end())
        {
            DI_WARNING("Cannot find the controller type: %s", controllerType.c_str());
            return;
        }

        SAFE_DELETE(it->second);
        mControllerFactories.erase(it);
    }
    
    DiParticleController* DiEffectManager::CreateController(const DiString& controllerType)
    {
        auto it = mControllerFactories.find(controllerType);

        if (it == mControllerFactories.end())
        {
            DI_WARNING("Cannot find the controller type: %s", controllerType.c_str());
            return nullptr;
        }

        return it->second->CreateController();
    }
    
    DiParticleController* DiEffectManager::CloneController(DiParticleController* affector)
    {
        if (!affector)
        {
            return nullptr;
        }

        DiParticleController* clonedAffector = CreateController(affector->GetControllerType());
        affector->CopyTo(clonedAffector);
        return clonedAffector;
    }
    
    void DiEffectManager::AddRendererFactory(DiParticleRendererFactory* factory)
    {
        DiString type = factory->GetRendererType();
        mRendererFactories[type] = factory;
    }
    
    DiParticleRendererFactory* DiEffectManager::GetRendererFactory(const DiString& rendererType)
    {
        auto it = mRendererFactories.find(rendererType);
        if (it != mRendererFactories.end())
        {
            return it->second;
        }
        return 0;
    }
    
    void DiEffectManager::RemoveRendererFactory(const DiString& rendererType)
    {
        auto it = mRendererFactories.find(rendererType);
        if (it == mRendererFactories.end())
        {
            DI_WARNING("Cannot find the renderer type: %s", rendererType.c_str());
            return;
        }

        mRendererFactories.erase(it);
    }
    
    void DiEffectManager::DestroyRendererFactory(const DiString& rendererType)
    {
        auto it = mRendererFactories.find(rendererType);
        if (it == mRendererFactories.end())
        {
            DI_WARNING("Cannot find the renderer type: %s", rendererType.c_str());
            return;
        }

        SAFE_DELETE(it->second);
        mRendererFactories.erase(it);
    }
    
    DiParticleRenderer* DiEffectManager::CreateRenderer(const DiString& rendererType)
    {
        auto it = mRendererFactories.find(rendererType);

        if (it == mRendererFactories.end())
        {
            DI_WARNING("Cannot find the renderer type: %s", rendererType.c_str());
            return nullptr;
        }

        return it->second->CreateRenderer();
    }
    
    DiParticleRenderer* DiEffectManager::CloneRenderer(DiParticleRenderer* renderer)
    {
        if (!renderer)
        {
            return 0;
        }

        DiParticleRenderer* clonedRenderer = CreateRenderer(renderer->GetRendererType());
        renderer->CopyTo(clonedRenderer);
        return clonedRenderer;
    }
    
    DiParticleSystemPtr DiEffectManager::CreateParticleSystemTemplate(const DiString& name)
    {
        DiString expName = name;
        while (mParticleSystemTemplates.find(expName) != mParticleSystemTemplates.end())
        {
            DI_INFO("Repetitive particle system template name: %s", name.c_str());
            expName = expName + "_copy";
        }

        DiParticleSystemPtr particleSystemTemplate = make_shared<DiParticleSystem>(expName);
        particleSystemTemplate->SetTemplateName(name);

        AddParticleSystemTemplate(expName, particleSystemTemplate);
        mLastCreatedTemplateName = expName;
        return particleSystemTemplate;
    }
    
    void DiEffectManager::ReplaceParticleSystemTemplate(const DiString& name, DiParticleSystemPtr system)
    {
        DiParticleSystemPtr systemTemplate = GetParticleSystemTemplate(name);
        if (systemTemplate)
        {
            *systemTemplate = *system;
            system->CopyTo(systemTemplate.get());
        }
    }
    
    const DiString& DiEffectManager::GetLastCreatedTemplateName(void)
    {
        return mLastCreatedTemplateName;
    }
    
    void DiEffectManager::AddParticleSystemTemplate(const DiString& name, DiParticleSystemPtr systemTemplate)
    {
        if (mParticleSystemTemplates.find(name) != mParticleSystemTemplates.end())
        {
            DI_WARNING("The effect template: %s has already existed", name.c_str());
            return;
        }

        mParticleSystemTemplates[name] = systemTemplate;
    }
    
    DiParticleSystemPtr DiEffectManager::GetParticleSystemTemplate(const DiString& templateName)
    {
        auto i = mParticleSystemTemplates.find(templateName);
        if (i != mParticleSystemTemplates.end())
            return i->second;

        return nullptr;
    }
    
    void DiEffectManager::DestroyParticleSystemTemplate(const DiString& templateName)
    {
        auto i = mParticleSystemTemplates.find(templateName);
        if (i != mParticleSystemTemplates.end())
        {
            mParticleSystemTemplates.erase(i);
            return;
        }
        
        DI_WARNING("Cannot locate the ps template: %s", templateName.c_str());
    }
    
    void DiEffectManager::ParticleSystemTemplateNames(DiVector<DiString>& v)
    {
        auto itEnd = mParticleSystemTemplates.end();
        for (auto it = mParticleSystemTemplates.begin(); it != itEnd; ++it)
        {
            v.push_back((*it).first);
        }
    }
    
    DiParticleSystemPtr DiEffectManager::CreateParticleSystem(const DiString& name,
        const DiString& templateName)
    {
        if (mParticleSystems.find(name) != mParticleSystems.end())
        {
            DI_WARNING("The effect: %s has already existed, template name: %s", name.c_str(), templateName.c_str());
            return nullptr;
        }

        auto system = CreateSystemImpl(name,templateName);
        system->SetTemplateName(templateName);
        mParticleSystems[name] = system;
        return system;
    }
    
    DiParticleSystemPtr DiEffectManager::CreateParticleSystem(const DiString& name)
    {
        if (mParticleSystems.find(name) != mParticleSystems.end())
        {
            DI_WARNING("The effect: %s has already existed", name.c_str());
            return nullptr;
        }

        auto system = CreateSystemImpl(name);
        mParticleSystems[name] = system;
        return system;
    }
    
    DiParticleSystemPtr DiEffectManager::GetParticleSystem(const DiString& name)
    {
        if (name == DiString::BLANK)
        {
            return nullptr;
        }

        auto i = mParticleSystems.find(name);
        if (i != mParticleSystems.end())
        {
            return i->second;
        }
    
        return nullptr;
    }
    
    void DiEffectManager::DestroyParticleSystem(DiParticleSystemPtr particleSystem)
    {
        if (!particleSystem)
            return;

        auto i = mParticleSystems.find(particleSystem->GetName());
        if (i != mParticleSystems.end())
            mParticleSystems.erase(i);
    }
    
    void DiEffectManager::DestroyParticleSystem(const DiString& particleSystemName)
    {
        DiParticleSystemPtr ps = nullptr;
        auto i = mParticleSystems.find(particleSystemName);
        if (i != mParticleSystems.end())
        {
            ps = i->second;
            mParticleSystems.erase(i);
            return;
        }
        DI_WARNING("Cannot find the particle system:%s",particleSystemName.c_str());
    }
    
    void DiEffectManager::DestroyAllParticleSystems()
    {
        auto t = mParticleSystems.begin();
        while ( t != mParticleSystems.end() )
        {
            auto particleSystem = t->second;
            ++t;
        }
        mParticleSystems.clear();
    }
    
    DiParticleSystemPtr DiEffectManager::CreateSystemImpl(const DiString& name)
    {
        return make_shared<DiParticleSystem>(name);
    }
    
    DiParticleSystemPtr DiEffectManager::CreateSystemImpl(const DiString& name, const DiString& templateName)
    {
        auto pTemplate = GetParticleSystemTemplate(templateName);
        if (!pTemplate)
        {
            DI_WARNING("Cannot find particle template: %s", templateName.c_str());
            return nullptr;
        }

        auto sys = make_shared<DiParticleSystem>(name);
        *sys = *pTemplate;
        pTemplate->CopyTo(sys.get());
        return sys;
    }
    
    DiDynamicAttribute* DiEffectManager::CreateDynamicAttribute(DiDynamicAttribute::DynamicAttributeType type)
    {
        switch (type)
        {
        case DiDynamicAttribute::DAT_FIXED:
            return DI_NEW DiAttributeFixed();
            break;

        case DiDynamicAttribute::DAT_RANDOM:
            return DI_NEW DiAttributeRandom();
            break;

        case DiDynamicAttribute::DAT_CURVED:
            return DI_NEW DiAttributeCurved();
            break;
        }
        return 0;
    }

    void DiEffectManager::InitFactories()
    {
        DiParticleRendererFactory* particleRendererFactory = NULL;
        particleRendererFactory = DI_NEW DiBillboardRendererFactory();
        AddRendererFactory(particleRendererFactory);

        DiParticleEmitterFactory* particleEmitterFactory = NULL;
        particleEmitterFactory = DI_NEW DiPointEmitterFactory();
        AddEmitterFactory(particleEmitterFactory);
        particleEmitterFactory = DI_NEW DiLineEmitterFactory();
        AddEmitterFactory(particleEmitterFactory);
        particleEmitterFactory = DI_NEW DiCircleEmitterFactory();
        AddEmitterFactory(particleEmitterFactory);
        particleEmitterFactory = DI_NEW DiBoxEmitterFactory();
        AddEmitterFactory(particleEmitterFactory);

        DiParticleControllerFactory* particleControllerFactory = NULL;
        particleControllerFactory = DI_NEW DiBoxColliderControllerFactory();
        AddControllerFactory(particleControllerFactory);
        particleControllerFactory = DI_NEW DiColorControllerFactory();
        AddControllerFactory(particleControllerFactory);
        particleControllerFactory = DI_NEW DiGeometryRotatorControllerFactory();
        AddControllerFactory(particleControllerFactory);
        particleControllerFactory = DI_NEW DiGravityControllerFactory();
        AddControllerFactory(particleControllerFactory);
        particleControllerFactory = DI_NEW DiJetControllerFactory();
        AddControllerFactory(particleControllerFactory);
        particleControllerFactory = DI_NEW DiLinearForceControllerFactory();
        AddControllerFactory(particleControllerFactory);
        particleControllerFactory = DI_NEW DiPlaneColliderControllerFactory();
        AddControllerFactory(particleControllerFactory);
        particleControllerFactory = DI_NEW DiRandomiserControllerFactory();
        AddControllerFactory(particleControllerFactory);
        particleControllerFactory = DI_NEW DiScaleControllerFactory();
        AddControllerFactory(particleControllerFactory);
        particleControllerFactory = DI_NEW DiSineForceControllerFactory();
        AddControllerFactory(particleControllerFactory);
        particleControllerFactory = DI_NEW DiSphereColliderControllerFactory();
        AddControllerFactory(particleControllerFactory);
        particleControllerFactory = DI_NEW DiVortexControllerFactory();
        AddControllerFactory(particleControllerFactory);
        particleControllerFactory = DI_NEW DiTextureRotatorControllerFactory();
        AddControllerFactory(particleControllerFactory);
    }

    void DiEffectManager::ReleaseFactories()
    {
        auto rfItEnd = mRendererFactories.end();
        for (auto rfIt = mRendererFactories.begin(); rfIt != rfItEnd; ++rfIt)
        {
            SAFE_DELETE(rfIt->second);
        }
        mRendererFactories.clear();

        auto efItEnd = mEmitterFactories.end();
        for (auto efIt = mEmitterFactories.begin(); efIt != efItEnd; ++efIt)
        {
            SAFE_DELETE(efIt->second);
        }
        mEmitterFactories.clear();

        auto cfItEnd = mControllerFactories.end();
        for (auto cfIt = mControllerFactories.begin(); cfIt != cfItEnd; ++cfIt)
        {
            SAFE_DELETE(cfIt->second);
        }
        mControllerFactories.clear();
    }

    void DiEffectManager::Update()
    {
        float deltatime = Driver->GetDeltaSecond();
        auto itEnd = mParticleSystems.end();
        for (auto it = mParticleSystems.begin(); it != itEnd; ++it)
        {
            it->second->Update(deltatime);
        }
    }

    void DiEffectManager::LoadEffects()
    {
        DiFxTokensParser parser;
        DiAssetManager::GetInstance().IterateArchive("fx", [&](const DiString& name, ArchivePtr arc){
            auto stream = arc->Open(name);
            parser.LoadEffects(stream);
        });
    }
}


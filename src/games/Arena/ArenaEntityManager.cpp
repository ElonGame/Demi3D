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

#include "ArenaPch.h"
#include "ArenaGameEntity.h"
#include "ArenaEntityManager.h"
#include "ArenaHero.h"

namespace Demi
{
    ArEntityManager::ArEntityManager()
    {
    }

    ArEntityManager::~ArEntityManager()
    {
        Release();
    }

    ArHeroEntityPtr ArEntityManager::CreateHero(ArObjID id, const DiString& configfile)
    {
        if (mHeroId == id && mHeroEntity)
            return mHeroEntity;

        mHeroEntity = make_shared<ArHeroEntity>();
        mHeroEntity->Init(GAME_HERO);
        mEntities[id] = mHeroEntity;

        mHeroEntity->SetID(id);
        mHeroEntity->InitComponents();
        mHeroEntity->InitAttribute(configfile);

        mHeroId = id;
        return mHeroEntity;
    }

    void ArEntityManager::Update(float dt)
    {
        auto itEnd = mEntities.end();
        for (auto it = mEntities.begin(); it != itEnd; it++)
        {
            auto entity = it->second;
            entity->Update(dt);
        }
    }

    ArGameEntityPtr ArEntityManager::CreateNPC(ArObjID id, const DiString& configfile)
    {
        auto entity = FindEntity(id);
        if (entity)
        {
            DI_WARNING("The entity[id = %d] has already existed!", id);
        }
        else
        {
            entity = make_shared<ArNPCEntity>();
            entity->Init(GAME_NPC);
            mEntities[id] = entity;

            entity->SetID(id);
            entity->InitComponents();
            entity->InitAttribute(configfile);
        }
        return entity;
    }

    ArGameEntityPtr ArEntityManager::FindEntity(ArObjID id)
    {
        auto it = mEntities.find(id);
        return it != mEntities.end() ? it->second : nullptr;
    }

    void ArEntityManager::Release()
    {
        mEntities.clear();
        mHeroEntity.reset();
        mHeroId = INVALID_OBJ_ID;
    }

    Demi::ArObjID ArAutoID::sID = 0;
}
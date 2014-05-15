
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

#ifndef BaseColliderController_h__
#define BaseColliderController_h__

#include "FxPrerequisites.h"
#include "ParticleController.h"
#include "ParticleControllerFactory.h"

namespace Demi
{
    class DEMI_FX_API DiBaseColliderController : public DiParticleController
    {
    public:
        // ���ӵ������������ײ��ʽ
        enum IntersectionType
        {
            IT_POINT,    //��������Ϊ�ʵ������ײ����
            IT_BOX        //��������Ϊ�г�/��/�߲�����box������ײ����
        };

        // ��ײ������ķ�����Ϊ����
        enum CollisionType
        {
            CT_NONE,        //�����κ���
            CT_BOUNCE,        //����
            CT_FLOW,        //������ײ����Χ��������
        };

        static const float                DEFAULT_BOUNCYNESS;
        static const float                DEFAULT_FRICTION;
        static const IntersectionType    DEFAULT_INTERSECTION_TYPE;
        static const CollisionType        DEFAULT_COLLISION_TYPE;

        DiBaseColliderController(void);

        virtual                        ~DiBaseColliderController(void){}

    public:

        const IntersectionType        GetIntersectionType(void) const;

        void                        SetIntersectionType(const IntersectionType& intersectionType);

        const CollisionType            GetCollisionType(void) const;

        void                        SetCollisionType(const CollisionType& collisionType);

        // Ħ����
        const float                    GetFriction(void) const;

        void                        SetFriction(const float friction);

        // ����ϵ��
        const float                    GetBouncyness(void) const;

        void                        SetBouncyness(const float bouncyness);

        virtual void                PreProcessParticles(DiParticleElement* particleTechnique, float timeElapsed);

        // ����AABB������ײ����
        void                        PopulateAlignedBox(DiAABB& box,
                                        const DiVec3& position, 
                                        const float width,
                                        const float height,
                                        const float depth);

        // ��ײ�����¼������ӵ���ת�ٶ�
        void                        CalculateRotationSpeedAfterCollision(DiParticle* particle);

        virtual void                CopyTo (DiParticleController* affector);

    protected:
        // Ħ������Ӱ�����ӵ���ת�ٶȲ���
        float                        mFriction;

        // ����ϵ����Ӱ�����ӵ��ٶȲ���
        float                        mBouncyness;

        IntersectionType            mIntersectionType;
        
        CollisionType                mCollisionType;
        
        float                        mVelocityScale;
    };
}

#endif // BaseColliderController_h__

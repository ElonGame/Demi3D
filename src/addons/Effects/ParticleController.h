
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

#ifndef ParticleController_h__
#define ParticleController_h__

#include "FxPrerequisites.h"
#include "Particle.h"

namespace Demi
{
    class DEMI_FX_API DiParticleController : public DiParticle
    {
    public:
        // �������Ŀ��Ʒ�ʽ�����ڼ��㲿�ֿ������ĸ��Ӳ���
        // ֻ�м����������Ż��漰�����ö��ֵ������������/����/������
        enum ControlSpecial
        {
            AFSP_DEFAULT,         //�����㶨��1.0
            AFSP_TTL_INCREASE,    //������ʱ�����
            AFSP_TTL_DECREASE     //������ʱ��ݼ�
        };

        static const bool            DEFAULT_ENABLED;
        static const DiVec3          DEFAULT_POSITION;
        static const ControlSpecial  DEFAULT_SPECIALISATION;

    public:

        DiParticleController(void);

        virtual                     ~DiParticleController(void) {}

    public:

        const ControlSpecial&       GetControllerSpecialisation(void) const {return mControllerSpecialisation;}

        void                        SetControllerSpecialisation(const ControlSpecial& affectSpecialisation);

        const DiString&             GetControllerType(void) const {return mControllerType;}

        void                        SetControllerType(const DiString& affectorType) {mControllerType = affectorType;}

        const DiString&             GetName(void) const {return mName;}

        void                        SetName(const DiString& name) {mName = name;}

        DiParticleElement*          GetParentElement(void) const {return mParentElement;}

        void                        SetParentElement(DiParticleElement* parentTechnique);

        virtual void                Prepare(DiParticleElement* particleTechnique){}

        virtual void                Unprepare(DiParticleElement* particleTechnique){}

        virtual void                NotifyStart (void);
                                    
        virtual void                NotifyStop (void){}
                                    
        virtual void                NotifyPause (void){}
                                    
        virtual void                NotifyResume (void){}

        virtual void                NotifyRescaled(const DiVec3& scale);

        virtual void                PreProcessParticles(DiParticleElement* particleTechnique, float timeElapsed){}

        virtual void                FirstParticle(DiParticleElement* particleTechnique, 
                                        DiParticle* particle, 
                                        float timeElapsed) {}

        virtual void                InitForEmission(void);

        virtual void                InitForExpiration(DiParticleElement* technique, float timeElapsed);

        virtual void                InitParticleForEmission(DiParticle* particle) {}

        // �������ӵ���ں�������Control����֮ǰ����
        void                        ProcessParticle(DiParticleElement* particleTechnique, 
                                        DiParticle* particle, 
                                        float timeElapsed, 
                                        bool firstParticle);

        virtual void                PostProcessParticles(DiParticleElement* technique, float timeElapsed){}

        // �������ĺ��Ĺ��ܣ�����һ������
        virtual void                Control(DiParticleElement* particleTechnique, 
                                        DiParticle* particle, 
                                        float timeElapsed) = 0;

        // �ų��ķ������������ܴ˿�����Ӱ��
        void                        AddEmitterToExclude(const DiString& emitterName);

        void                        RemoveEmitterToExclude(const DiString& emitterName);

        void                        RemoveAllEmittersToExclude(void);

        const DiList<DiString>&     GetEmittersToExclude(void) const;

        bool                        HasEmitterToExclude(const DiString& emitterName);

        virtual void                CopyTo(DiParticleController* affector);

        virtual void                CopyParentTo(DiParticleController* affector);

        const DiVec3&               GetDerivedPosition(void);

        // ����ControlSpecialö��ֵ���㸽�Ӳ���
        float                       CalculateAffectSpecialisationFactor (const DiParticle* particle);

    protected:
        DiParticleElement*          mParentElement;

        DiString                    mControllerType;

        DiString                    mName;

        ControlSpecial              mControllerSpecialisation;

        DiList<DiString>            mExcludedEmitters;

        // �������������ֵ
        DiVec3                      mControllerScale;
    };
}

#endif // ParticleController_h__

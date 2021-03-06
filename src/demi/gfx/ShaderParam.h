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

#ifndef ShaderParam_h__
#define ShaderParam_h__

#include "GfxPrerequisites.h"
#include "Texture.h"
#include "GpuProgram.h"
#include "ShaderProgram.h"

namespace Demi 
{
    class DI_GFX_API DiShaderParameter : public DiBase
    {
    public:

        friend class DiMaterial;

    public:

        DiShaderParameter(DiMaterial& mat);

        virtual ~DiShaderParameter();

        typedef DiStrHash<DiAny>  ShaderParamMap;

        enum ParamType
        {
            VARIABLE_FLOAT = 0,
            VARIABLE_FLOAT2,
            VARIABLE_FLOAT3,
            VARIABLE_COLOR,
            VARIABLE_FLOAT4,
            VARIABLE_FLOAT4_ARRAY,
            VARIABLE_MAT4,

            VARIABLE_SAMPLER2D,
            VARIABLE_SAMPLERCUBE,

            NUM_VARIABLE_TYPES
        };

    public:

        void                    AddParameter(ParamType type, const DiString& name);

        const ShaderParamMap&   GetShaderParams(ParamType type) { return mShaderParams[type]; }

        virtual void            LoadParameters() = 0;

        void                    CloneVarsTo(DiShaderParameter* ps);

        void                    WriteFloat4(const DiString& name, DiVec4 vec4);

        void                    WriteColor(const DiString& name, DiColor vec4);

        void                    WriteFloat4Array(const DiString& name, DiVec4* vec4,uint32 size);

        void                    WriteMatrix4(const DiString& name, DiMat4 mat4);

        void                    WriteFloat3(const DiString& name, DiVec3 vec3);

        void                    WriteFloat2(const DiString& name, DiVec2 vec2);

        void                    WriteFloat(const DiString& name, float);

        DiTexturePtr            WriteTexture2D(const DiString& name, const DiString& textureName);

        void                    WriteTexture2D(const DiString& name,DiTexturePtr texture);

        DiTexturePtr            WriteTextureCUBE(const DiString& name, const DiString& textureName);

        void                    WriteTextureCUBE(const DiString& name,DiTexturePtr texture);

        // bind all parameters
        virtual void            Bind() const = 0;

        bool                    HasVariableType(ParamType varType);

        static DiAny            GetDefault(ParamType type);

    public:

        virtual void            _BindBuiltin(const DiString& name) = 0;

        virtual void            _BindTexture2Ds() = 0;

    protected:

        DiMaterial&             mMaterial;

        ShaderParamMap          mShaderParams[NUM_VARIABLE_TYPES];
    };
}

#endif // ShaderParam_h__
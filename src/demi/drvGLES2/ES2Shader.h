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

#ifndef GLES2Shader_h__
#define GLES2Shader_h__

#include "ES2Prerequisites.h"
#include "GpuProgram.h"
#include "VertexFormat.h"
#include <functional>

namespace Demi
{
    struct DiGLES2ShaderConstant
    {
        GLuint program;
        GLint  location;
        GLenum type;
        uint32 unit; // for samplers
    };

    class DiGLES2ShaderInstance : public DiShaderInstance
    {
    public:

        DiGLES2ShaderInstance(DiShaderType type, DiShaderProgram* prog);

        ~DiGLES2ShaderInstance();

    public:

        bool                Compile(const DiString& code);

        void                Bind(const DiShaderEnvironment& shaderEnv);

        void                LinkToProgramObject(const GLuint programObject);

        void                UnlinkToProgramObject(const GLuint programObject);

        DiShaderProgram*    GetShaderProgram() { return mShaderProgram; }

        void                Release();

        GLuint              GetShaderHandle() { return mShaderHandle; }
        
        GLuint              GetProgramHandle() { return mGLProgramHandle; }

        DiShaderType        GetType()
        {
            return mType;
        }

        /** Return the programs link status
        Only used when programs are linked separately with GL_EXT_separate_shader_objects.
        */
        GLint               IsLinked(void) { return mLinked; }

        /** Set the programs link status
        Only used when programs are linked separately with GL_EXT_separate_shader_objects.
        */
        void                SetLinked(GLint flag) { mLinked = flag; }

    public:

        static DiString     LogObjectInfo(const DiString& msg, const GLuint obj);

    private:

        void                PorcessShaders(const DiString& code);

        DiShaderProgram*    mShaderProgram;

        GLuint              mShaderHandle;

        GLuint              mGLProgramHandle;

        DiShaderType        mType;

        GLint               mCompiled;

        DiString            mProcessedShader;

        GLint               mLinked;
    };

    //////////////////////////////////////////////////////////////////////////

    class DiGLUniforms
    {
    public:

        typedef std::function<void(const DiShaderEnvironment*, DiGLES2ShaderConstant*)> BindingFunc;

        typedef DiStrHash<BindingFunc> UniformFuncs;

        static void InitUniformFuncs();

        static UniformFuncs msUniformFuncs;
    };

    //////////////////////////////////////////////////////////////////////////

    class DiGLES2ShaderLinker : public DiBase
    {
    public:

        DiGLES2ShaderLinker(DiGLES2ShaderInstance* vs, DiGLES2ShaderInstance* ps);

        ~DiGLES2ShaderLinker();

    public:

        void                   Link();

        void                   Bind();

        void                   LoadConstants(DiGLES2ShaderParam* params);

        DiGLES2ShaderConstant* GetConstant(const DiString& constname);

        DiGLES2ShaderConstant* GetSampler(const DiString& constname);

        bool                   HasConstant(const DiString& constname);

        bool                   HasSampler(const DiString& samplername);

        GLuint                 GetGLHandle() const { return mGLHandle; }

    private:

        struct CustomAttribute
        {
            DiString name;
            GLuint   attrib;
            CustomAttribute(const DiString& _name, GLuint _attrib)
                :name(_name), attrib(_attrib) {}
        };

        static CustomAttribute msCustomAttributes[];

    private:
        
        GLuint              mGLHandle;

        DiGLES2ShaderInstance* mVS;

        DiGLES2ShaderInstance* mPS;

        GLint               mLinked;

        typedef DiStrHash<DiGLES2ShaderConstant*> Consts;
        typedef DiStrHash<DiGLES2ShaderConstant*> Samplers;

        Consts              mConsts;

        Samplers            mSamplers;
    };
}

#endif // GLShader_h__
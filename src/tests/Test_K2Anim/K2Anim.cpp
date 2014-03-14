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

#include "K2Anim.h"

struct matrix34
{
    float m00, m01, m02;
    float m10, m11, m12;
    float m20, m21, m22;
    float m30, m31, m32;
};

DiMat4 converMat(const matrix34& m)
{
    DiMat4 ret(
               m.m00, m.m01, m.m02, 0,
               m.m10, m.m11, m.m12, 0,
               m.m20, m.m21, m.m22, 0,
               m.m30, m.m31, m.m32, 1);
    ret = ret.transpose();
    return ret;
}

bool check_fourcc_chunk(FILE* fp, char* sig)
{
    char hed[4];
    fread(hed, sizeof(char), 4, fp);
    if (sig[0] != hed[0] ||
        sig[1] != hed[1] ||
        sig[2] != hed[2] ||
        sig[3] != hed[3])
    {
        return false;
    }
    return true;
}

bool check_fourcc_chunk(char* hed, char* sig)
{
    if (sig[0] != hed[0] ||
        sig[1] != hed[1] ||
        sig[2] != hed[2] ||
        sig[3] != hed[3])
    {
        return false;
    }
    return true;
}


int read_int(FILE* fp)
{
    int num = 0;
    fread(&num, sizeof(int), 1, fp);
    return num;
}

char read_char(FILE* fp)
{
    char num = 0;
    fread(&num, sizeof(char), 1, fp);
    return num;
}

unsigned char read_uchar(FILE* fp)
{
    unsigned char num = 0;
    fread(&num, sizeof(unsigned char), 1, fp);
    return num;
}

float read_float(FILE* fp)
{
    float num = 0;
    fread(&num, sizeof(float), 1, fp);
    return num;
}

void read_matrix34(FILE* fp, matrix34* mat)
{
    fread(mat, sizeof(matrix34), 1, fp);
}

void read_string(FILE* fp, char* are, int length)
{
    fread(are, length, 1, fp);
    are[length] = '\0';
}

enum ClipTags
{
    MKEY_X,
    MKEY_Y,
    MKEY_Z,
    MKEY_PITCH,
    MKEY_ROLL,
    MKEY_YAW,
    MKEY_VISIBILITY,
    MKEY_SCALE_X,
    MKEY_SCALE_Y,
    MKEY_SCALE_Z
};

size_t maxClipKeys(Clip& c)
{
    return std::max(c.pos.size(), std::max(c.rot.size(), c.scale.size()));
}

void updateClipSize(Clip& c, int keytype, int size)
{
    switch (keytype)
    {
        case MKEY_X:
        case MKEY_Y:
        case MKEY_Z:
            if (c.pos.size() != size)
                c.pos.resize(size, DiVec3::ZERO);
            return;
        case MKEY_PITCH:
        case MKEY_ROLL:
        case MKEY_YAW:
            if (c.rot.size() != size)
                c.rot.resize(size, DiVec3::ZERO);
            return;
        case MKEY_VISIBILITY:
            if (c.vis.size() != size)
                c.vis.resize(size, 0);
            return;
        case MKEY_SCALE_X:
        case MKEY_SCALE_Y:
        case MKEY_SCALE_Z:
            if (c.scale.size() != size)
                c.scale.resize(size, DiVec3::UNIT_SCALE);
            return;
        default:
            break;
    }
}

void updateClip(Clip& c, int keytype, int keyID, float val, int vis)
{
    switch (keytype)
    {
        case MKEY_X:
            c.pos[keyID].x = val; return;
        case MKEY_Y:
            c.pos[keyID].y = val; return;
        case MKEY_Z:
            c.pos[keyID].z = val; return;
        case MKEY_PITCH:
            c.rot[keyID].x = val; return;
        case MKEY_ROLL:
            c.rot[keyID].y = val; return;
        case MKEY_YAW:
            c.rot[keyID].z = val; return;
        case MKEY_VISIBILITY:
            c.vis[keyID] = vis; return;
        case MKEY_SCALE_X:
            c.scale[keyID].x = val; return;
        case MKEY_SCALE_Y:
            c.scale[keyID].x = val; return;
        case MKEY_SCALE_Z:
            c.scale[keyID].x = val; return;
        default:
            break;
    }
}


void readClip(FILE* fp, DiVector<Clip>& clips)
{
    if (!check_fourcc_chunk(fp, "bmtn"))
    {
        printf("unknow clip chunk\n");
        return;
    }

    int chunkSize = read_int(fp);

    int boneindex = read_int(fp);
    int keytype = read_int(fp);
    int numkeys = read_int(fp);

    unsigned char nameLen = read_uchar(fp);
    char* name = new char[nameLen + 1];
    read_string(fp, name, nameLen);
    read_uchar(fp);

    //printf("boneindex: %d, keytype: %d, numkeys: %d\n", boneindex, keytype, numkeys);
    //printf("name: %s\n", name);

    Clip& c = clips[boneindex];
    updateClipSize(c, keytype, numkeys);

    if (keytype == MKEY_VISIBILITY)
    {
        unsigned char* data = new unsigned char[numkeys];
        fread(data, numkeys, 1, fp);
        for (int i = 0; i < numkeys; i++)
            updateClip(c, keytype, i, 0, data[i]);
        delete[] data;
    }
    else
    {
        float* data = new float[numkeys];
        fread(data, numkeys * sizeof(float), 1, fp);
        for (int i = 0; i < numkeys; i++)
            updateClip(c, keytype, i, data[i], 0);
        delete[] data;
    }

    delete[] name;
}

int order = 0;
DiQuat convEuler(DiVec3& v)
{
    DiMat3 rotmat;
    switch (order)
    {
    case 0:
        rotmat.FromEulerAnglesXYZ(DiDegree(v.x), DiDegree(v.y), DiDegree(v.z));
        break;
    case 1:
        rotmat.FromEulerAnglesXZY(DiDegree(v.x), DiDegree(v.y), DiDegree(v.z));
        break;
    case 2:
        rotmat.FromEulerAnglesYXZ(DiDegree(v.x), DiDegree(v.y), DiDegree(v.z));
        break;
    case 3:
        rotmat.FromEulerAnglesYZX(DiDegree(v.x), DiDegree(v.y), DiDegree(v.z));
        break;
    case 4:
        rotmat.FromEulerAnglesZXY(DiDegree(v.x), DiDegree(v.y), DiDegree(v.z));
        break;
    case 5:
        rotmat.FromEulerAnglesZYX(DiDegree(v.x), DiDegree(v.y), DiDegree(v.z));
        break;
    }
    DiQuat q;
    q.FromRotationMatrix(rotmat);
    return q;
}

////////////////////////////////////////////////////////////////////////

K2Anim::K2Anim(const DiString& baseDir, DiSceneManager* sm)
  : mBaseDir(baseDir)
  , mSm(sm)
  , mNumFrames(0)
  , mRootBone(nullptr)
{
    mBonesHelper = make_shared<DiDebugHelper>();
    DiMaterialPtr helpermat = DiMaterial::QuickCreate("basic_v", "basic_p", SHADER_FLAG_USE_COLOR);
    helpermat->SetDepthCheck(false);
    mBonesHelper->SetMaterial(helpermat);
    sm->GetRootNode()->CreateChild()->AttachObject(mBonesHelper);

    mClipsHelper = make_shared<DiDebugHelper>();
    mClipsHelper->SetMaterial(helpermat);
    sm->GetRootNode()->CreateChild()->AttachObject(mClipsHelper);
}

K2Anim::~K2Anim()
{
}

void K2Anim::_LoadBones(const DiString& model)
{
    DiString realfile = mBaseDir + model;
    printf("Loading bones\n");
    FILE* fp = fopen(realfile.c_str(), "rb");
    
    if (!check_fourcc_chunk(fp, "SMDL"))
    {
        printf("unknow file format\n");
        return;
    }
    
    if (!check_fourcc_chunk(fp, "head"))
    {
        printf("file does not start with head chunk\n");
        return;
    }
    
    // head size
    int temp = read_int(fp);
    
    int version = read_int(fp);
    int num_meshes = read_int(fp);
    int num_sprites = read_int(fp);
    int num_surfs = read_int(fp);
    int num_bones = read_int(fp);
    
    printf("%d bones\n", num_bones);
    
    float minx, miny, minz;
    float maxx, maxy, maxz;
    
    minx = read_float(fp);
    miny = read_float(fp);
    minz = read_float(fp);
    
    maxx = read_float(fp);
    maxy = read_float(fp);
    maxz = read_float(fp);
    
    if (!check_fourcc_chunk(fp, "bone"))
    {
        printf("no bone chunk\n");
        return;
    }
    
    // bone trunk size
    read_int(fp);
    
    mBones.clear();
    mParents.clear();
    mBoneNodes.clear();
    for (int id = 0; id < num_bones; id++)
    {
        int parent_bone_index = read_int(fp);
        mParents.push_back(parent_bone_index);
        printf("bone:%2d, parent:%2d\n", id, parent_bone_index);
        
        matrix34 inv_matrix;
        matrix34 matrix;
        read_matrix34(fp, &inv_matrix);
        read_matrix34(fp, &matrix);
        
        char length = read_char(fp);
        char name[255];
        read_string(fp, name, length);
        
        read_char(fp);
        
        DiMat4 mat = converMat(matrix);
        Trans t;
        mat.decomposition(t.pos, t.scale, t.rot);
        mBones.push_back(t);

        mBoneNodes.push_back(new DiNode());
        mBoneIds[mBoneNodes.back()] = id;
    }

    for (size_t i = 0; i < mBoneNodes.size(); ++i)
    {
        DiNode* b = mBoneNodes[i];
        if (mParents[i] >= 0)
        {
            DiNode* parent = mBoneNodes[mParents[i]];
            parent->AddChild(b);
        }
        else
            mRootBone = b;
    }

    fclose(fp);
}

void K2Anim::_LoadClips(const DiString& clip)
{
    DiString realfile = mBaseDir + clip;
    FILE* fp = fopen(realfile.c_str(), "rb");

    if (!check_fourcc_chunk(fp, "CLIP"))
    {
        printf("unknow file format\n");
        return;
    }

    if (!check_fourcc_chunk(fp, "head"))
    {
        printf("file does not start with head chunk\n");
        return;
    }

    int what = read_int(fp);
    int version = read_int(fp); // should be 2?
    int num_bones = read_int(fp);
    mNumFrames = read_int(fp);

    printf("num bones: %d\n", num_bones);
    printf("num frames: %d\n", mNumFrames);

    mClips.clear();
    for (int i = 0; i < num_bones; i++)
    {
        mClips.push_back(Clip(i));
    }

    while (!feof(fp))
    {
        readClip(fp, mClips);
    }

    fclose(fp);
}

void K2Anim::Load(const DiString& model, const DiString& clip)
{
    _LoadBones(model);
    _LoadClips(clip);
}

void K2Anim::_UpdateBonesHelper()
{
    mBonesHelper->Clear();
    uint32 numBons = mBones.size();
    for (uint32 i = 0; i < numBons; ++i)
    {
        DiVec3 pos = mBones[i].pos;
        DiVec3 posParent = mParents[i] >= 0 ? mBones[mParents[i]].pos : pos;

        mBonesHelper->AddLine(pos, posParent, DiColor::Yellow);
    }
}

void K2Anim::_UpdateClipsHelper()
{
    mClipsHelper->Clear();
    static float key = 0;
    key += 0.1f;
    if (key >= mNumFrames)
        key = 0;
    
    DiVector<DiNode*> level;
    level.push_back(mRootBone);
    while (!level.empty())
    {
        DiVector<DiNode*> next;
        for (auto i = level.begin(); i != level.end(); ++i)
        {
            DiNode* bone = *i;
            int id = mBoneIds[bone];

            if (id < mClips.size())
            {
                bone->SetPosition(mClips[id].getPos(key));
                bone->SetScale(mClips[id].getScale(key));
                DiQuat rot = convEuler(mClips[id].getRot(key));
                bone->SetOrientation(rot);
            }

            size_t cn = bone->GetChildrenNum();
            for (size_t c = 0; c < cn; c++)
                next.push_back((DiBone*)bone->GetChild(c));
        }
        level = next;
    }

    uint32 numBons = mBoneNodes.size();
    for (uint32 i = 0; i < numBons; ++i)
    {
        DiNode* b = mBoneNodes[i];
        DiNode* p = b->GetParent();
        DiVec3 pos = b->GetDerivedPosition();
        DiVec3 posParent = p ? (p->GetDerivedPosition()) : pos;
        mClipsHelper->AddLine(pos, posParent, DiColor::Red);
    }
}

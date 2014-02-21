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
#include "GfxPch.h"
#include "SceneManager.h"
#include "GfxDriver.h"
#include "RenderPipeline.h"
#include "Camera.h"
#include "CullNode.h"
#include "AssetManager.h"
#include "CullNode.h"
#include "InstanceManager.h"
#include "OctreeQuery.h"
#include "RenderWindow.h"
#include "RenderTarget.h"
#include "Skybox.h"
#include "GfxDriver.h"

namespace Demi 
{
    void DiVisibleObjs::UpdateAll(DiCamera* camera)
    {
        std::for_each(objs.begin(), objs.end(),
            [camera](DiTransUnitPtr obj) { obj->Update(camera); });
    }

    void DiVisibleObjs::AddToBatch(DiRenderPipeline* pipeline)
    {
        std::for_each(objs.begin(), objs.end(),
        [pipeline](DiTransUnitPtr obj) 
        { 
            obj->AddToBatchGroup(pipeline->GetBatchGroup(obj->GetBatchGroup()));
        });
    }

    enum Intersection
    {
        OUTSIDE   = 0,
        INSIDE    = 1,
        INTERSECT = 2
    };

    Intersection intersect( const DiRay &one, const DiAABB &two )
    {
        DiSceneManager::intersect_call++;
        
        if (two.IsNull())
            return OUTSIDE;
        
        if (two.IsInfinite()) 
            return INTERSECT;

        bool inside = true;
        const DiVec3& twoMin = two.GetMinimum();
        const DiVec3& twoMax = two.GetMaximum();
        DiVec3 origin = one.getOrigin();
        DiVec3 dir = one.getDirection();

        DiVec3 maxT(-1, -1, -1);

        int i = 0;
        for(i=0; i<3; i++ )
        {
            if( origin[i] < twoMin[i] )
            {
                inside = false;
                if( dir[i] > 0 )
                    maxT[i] = (twoMin[i] - origin[i])/ dir[i];
            }
            else if( origin[i] > twoMax[i] )
            {
                inside = false;
                if( dir[i] < 0 )
                    maxT[i] = (twoMax[i] - origin[i]) / dir[i];
            }
        }

        if( inside )
            return INTERSECT;
        
        int whichPlane = 0;
        if( maxT[1] > maxT[whichPlane])
            whichPlane = 1;

        if( maxT[2] > maxT[whichPlane])
            whichPlane = 2;

        if( ((int)maxT[whichPlane]) & 0x80000000 )
            return OUTSIDE;

        for(i=0; i<3; i++ )
        {
            if( i!= whichPlane )
            {
                float f = origin[i] + maxT[whichPlane] * dir[i];
                if ( f < (twoMin[i] - 0.00001f) ||
                    f > (twoMax[i] +0.00001f ) )
                {
                    return OUTSIDE;
                }
            }
        }

        return INTERSECT;

    }

    Intersection intersect( const DiPlaneBoundedVol &one, const DiAABB &two )
    {
        DiSceneManager::intersect_call++;
        if (two.IsNull()) 
            return OUTSIDE;
        if (two.IsInfinite()) 
            return INTERSECT;

        DiVec3 centre = two.GetCenter();
        
        DiVec3 halfSize = two.GetHalfSize();

        bool all_inside = true;
        DiCamera::PlaneList::const_iterator i, iend;
        iend = one.planes.end();
        for (i = one.planes.begin(); i != iend; ++i)
        {
            const DiPlane& plane = *i;

            DiPlane::Side side = plane.getSide(centre, halfSize);
            if(side == one.outside)
            {
                return OUTSIDE;
            }
            if(side == DiPlane::BOTH_SIDE)
            {
                all_inside = false; 
            }
        }

        if ( all_inside )
        {
            return INSIDE;
        }
        else
        {
            return INTERSECT;
        }
    }

    Intersection intersect( const DiAABB &one, const DiAABB &two )
    {
        DiSceneManager::intersect_call++;
        if (one.IsNull() || two.IsNull()) 
            return OUTSIDE;

        if (one.IsInfinite()) 
            return INSIDE;

        if (two.IsInfinite()) 
            return INTERSECT;

        const DiVec3& insideMin = two.GetMinimum();
        const DiVec3& insideMax = two.GetMaximum();

        const DiVec3& outsideMin = one.GetMinimum();
        const DiVec3& outsideMax = one.GetMaximum();

        if (    insideMax.x < outsideMin.x ||
            insideMax.y < outsideMin.y ||
            insideMax.z < outsideMin.z ||
            insideMin.x > outsideMax.x ||
            insideMin.y > outsideMax.y ||
            insideMin.z > outsideMax.z )
        {
            return OUTSIDE;
        }

        bool full = ( insideMin.x > outsideMin.x &&
            insideMin.y > outsideMin.y &&
            insideMin.z > outsideMin.z &&
            insideMax.x < outsideMax.x &&
            insideMax.y < outsideMax.y &&
            insideMax.z < outsideMax.z );

        if ( full )
            return INSIDE;
        else
            return INTERSECT;

    }

    Intersection intersect( const DiSphere &one, const DiAABB &two )
    {
        DiSceneManager::intersect_call++;
        if (two.IsNull()) 
            return OUTSIDE;

        if (two.IsInfinite()) 
            return INTERSECT;

        float sradius = one.getRadius();

        sradius *= sradius;

        DiVec3 scenter = one.getCenter();

        const DiVec3& twoMin = two.GetMinimum();
        const DiVec3& twoMax = two.GetMaximum();

        float s, d = 0;

        DiVec3 mndistance = ( twoMin - scenter );
        DiVec3 mxdistance = ( twoMax - scenter );

        if ( mndistance.squaredLength() < sradius &&
            mxdistance.squaredLength() < sradius )
        {
            return INSIDE;
        }

        for ( int i = 0 ; i < 3 ; i++ )
        {
            if ( scenter[ i ] < twoMin[ i ] )
            {
                s = scenter[ i ] - twoMin[ i ];
                d += s * s;
            }

            else if ( scenter[ i ] > twoMax[ i ] )
            {
                s = scenter[ i ] - twoMax[ i ];
                d += s * s;
            }

        }

        bool partial = ( d <= sradius );

        if ( !partial )
            return OUTSIDE;
        else
            return INTERSECT;
    }

    //////////////////////////////////////////////////////////////////////////

    DiSceneManager::DiSceneManager(DiRenderWindow* parentWnd)
        :mTerrain(nullptr),
        mParentWindow(parentWnd),
        mOctree(nullptr),
        mCurrentRenderPass(GEOMETRY_PASS),
        mLastFrameNumber(0),
        mSkybox(nullptr),
        mAmbientColor(0.3f,0.3f,0.3f),
        mCameraPool(this)
    {
        mPipeline = Driver->GetPipeline();
        mRootNode = DI_NEW DiCullNode(this,"_root");
        mCamera   = CreateCamera("_sm_camera");
        mOctree   = make_shared<DiOctree>( nullptr, nullptr, 0xffff );

        mBox = DiAABB( -10000, -10000, -10000, 10000, 10000, 10000 );
        mMaxDepth = 8; 

        mOctree->mBox = mBox;
        DiVec3 min = mBox.GetMinimum();
        DiVec3 max = mBox.GetMaximum();
        mOctree->mHalfSize = ( max - min ) / 2;
        
        mSkybox = DI_NEW DiSkybox(this);
    }

    DiSceneManager::~DiSceneManager(void)
    {
        DestroyTerrain();

        ClearNodes();
        DestroyCamera("_sm_camera");

        SAFE_DELETE(mSkybox);
    }

    DiCullNode* DiSceneManager::CreateNode()
    {
        DiCullNode* node = DI_NEW DiCullNode(this);
        mSceneNodes[node->GetName()] = node;
        return node;
    }

    DiCullNode* DiSceneManager::CreateNode( const DiString& name )
    {
        if (mSceneNodes.find(name) != mSceneNodes.end())
        {
            DI_WARNING("Cannot create node : %s, it is in the list already.",name.c_str());
            return nullptr;
        }

        DiCullNode* node = DI_NEW DiCullNode(this,name);
        mSceneNodes[name] = node;
        return node;
    }

    void DiSceneManager::DestroyNode( DiCullNode* node )
    {
        if ( node )
        {
            RemoveOctreeNode( node );
        }

        DestroyNode(node->GetName());
    }

    void DiSceneManager::DestroyNode( const DiString& name )
    {
        NodeList::iterator i = mSceneNodes.find(name);

        if (i == mSceneNodes.end())
        {
            DI_ERROR("Cannot find the node %s",name.c_str());
        }

        if (i->second != NULL)
            RemoveOctreeNode(i->second);

        DiNode* parentNode = i->second->GetParent();
        if (parentNode)
        {
            parentNode->RemoveChild(i->second);
        }
        delete i->second;
        mSceneNodes.erase(i);
    }

    bool DiSceneManager::HasSceneNode( const DiString& name )
    {
        return (mSceneNodes.find(name) != mSceneNodes.end());
    }

    void DiSceneManager::WalkTree(DiCamera* camera, DiOctreePtr octant,bool foundvisible)
    {
        if (octant->NumNodes() == 0)
            return;

        DiCamera::Visibility v = DiCamera::NONE;

        if ( foundvisible )
            v = DiCamera::FULL;
        else if ( octant == mOctree )
            v = DiCamera::PARTIAL;
        else
        {
            DiAABB box;
            octant->GetCullBounds( &box );
            v = camera->GetVisibility( box );
        }

        bool onlyShadowCaster = mCurrentRenderPass == SHADOW_PASS;

        if ( v != DiCamera::NONE )
        {
            bool vis = true;
            for (DiCullNode* node = octant->mNodes.mHead; node; node = node->mNext)
            {
                const DiAABB& nodeBounds = node->GetWorldAABB();

                if (v == DiCamera::PARTIAL)
                    vis = camera->IsVisible(nodeBounds);

                if (vis)
                {
                    // evaluate the Z range and update the scene bounds
                    if (nodeBounds.IsFinite())
                    {
                        mBox.Merge(nodeBounds);
                        
                        DiVec3 viewDir = camera->GetDerivedOrientation() * DiVec3::UNIT_Z;
                        DiVec3 absViewZ = viewDir.abs();
                        DiVec3 center = nodeBounds.GetCenter();
                        float viewCenterZ = viewDir.dotProduct(center) + camera->GetDerivedPosition().z;
                        DiVec3 edge = nodeBounds.GetHalfSize();
                        float viewEdgeZ = absViewZ.dotProduct(edge);
                        float minZ = viewCenterZ - viewEdgeZ;
                        float maxZ = viewCenterZ + viewEdgeZ;
                        mZRange.x = DiMath::Min(mZRange.x, minZ);
                        mZRange.y = DiMath::Max(mZRange.y, maxZ);
                    }

                    node->ProcessVisibleObjects([=](DiTransUnitPtr tu)
                    {
                        if (!onlyShadowCaster || tu->GetShadowCastEnable())
                        {
                            mVisibleObjects.objs.push_back(tu);
                            tu->NotifyCurrentCamera(camera);
                        }
                    });
                }
            }
        }

        DiOctreePtr child;
        bool childfoundvisible = (v == DiCamera::FULL);

        for (uint16 i = 0; i < 8; i++)
            if ((child = octant->mChildren[i]))
                WalkTree(camera, child, childfoundvisible);
    }

    void DiSceneManager::ClearNodes()
    {
        DI_INFO("Cleanning scene nodes..");

        mRootNode->RemoveAllChildren();
        mRootNode->DetachAllObjects();

        for (auto it = mSceneNodes.begin(); it != mSceneNodes.end(); ++it)
        {
            SAFE_DELETE(it->second);
        }
        mSceneNodes.clear();
    }

    bool DiSceneManager::LoadScene(const DiString& scene)
    {
        DI_INFO("Loading scene %s..", scene.c_str());

        DestroyTerrain();
        mCurrentScene = scene;
        return true;
    }

    void DiSceneManager::DestroyTerrain()
    {
        if (!mTerrain)
            return;
        
        DI_INFO("Releasing terrain..");

        mRootNode->DetachObject(mTerrain);
        mTerrain->Unload();
    }

    void DiSceneManager::PreUpdate()
    {
        UpdateDirtyInstanceManagers();
        mSkybox->Update();
        mRootNode->_Update(true, false);
    }

    void DiSceneManager::Cull(DiCamera* cam)
    {
        // clear all culled objects and lights
        mVisibleObjects.objs.clear();
        mVisibleLights.Clear();

        mZRange = DiVec2(DiMath::POS_INFINITY, 0);
        mBox.SetNull();

        WalkTree(cam, mOctree, false);
    }

    void DiSceneManager::UnloadScene()
    {
        DestroyTerrain();
        mCurrentScene.clear();
    }

    bool DiSceneManager::HasScene() const
    {
        return mTerrain != NULL;
    }

    bool DiSceneManager::LoadTerrain()
    {
        if (!mCurrentScene.empty())
        {
            mTerrain = make_shared<DiTerrainMap>(this);
            mTerrain->Load(mCurrentScene);
            mRootNode->AttachObject(mTerrain);
            return true;
        }
        return false;
    }

    DiScenePtr DiSceneManager::GetCurrentScene()
    {
        if (mCurrentScene.empty())
        {
            return DiScenePtr();
        }
        return DiAssetManager::GetInstance(
            ).GetAsset<DiScene>(mCurrentScene);
    }

    DiCullNode* DiSceneManager::GetSceneNode( const DiString& name )
    {
        NodeList::iterator it = mSceneNodes.find(name);
        if (it != mSceneNodes.end())
        {
            return it->second;
        }
        else
        {
            return NULL;
        }
    }

    void DiSceneManager::DirtyInstanceManager( DiInstanceManager *dirtyManager )
    {
        mDirtyInstanceManagers.push_back( dirtyManager );
    }

    void DiSceneManager::UpdateDirtyInstanceManagers()
    {
        if (mDirtyInstanceManagers.empty())
            return;

        mDirtyInstanceMgrsTmp.insert( mDirtyInstanceMgrsTmp.end(), mDirtyInstanceManagers.begin(),
            mDirtyInstanceManagers.end() );
        mDirtyInstanceManagers.clear();

        while( !mDirtyInstanceMgrsTmp.empty() )
        {
            auto itor = mDirtyInstanceMgrsTmp.begin();
            auto end  = mDirtyInstanceMgrsTmp.end();

            while( itor != end )
            {
                (*itor)->UpdateDirtyBatches();
                ++itor;
            }

            mDirtyInstanceMgrsTmp.clear();

            mDirtyInstanceMgrsTmp.insert( mDirtyInstanceMgrsTmp.end(), mDirtyInstanceManagers.begin(),
                mDirtyInstanceManagers.end() );
            mDirtyInstanceManagers.clear();
        }
    }

    void DiSceneManager::UpdateOctreeNode( DiCullNode* onode)
    {
        const DiAABB& box = onode->GetWorldAABB();

        if ( box.IsNull() )
            return;

        if (!mOctree)
            return;

        if ( !onode->GetOctant() )
        {
            if ( !onode->IsIn( mOctree->mBox ) )
                mOctree->AddNode( onode );
            else
                AddOctreeNode( onode, mOctree );
            return;
        }

        if ( !onode->IsIn( onode->GetOctant()->mBox ) )
        {
            RemoveOctreeNode( onode );

            if ( !onode->IsIn( mOctree->mBox ) )
                mOctree->AddNode( onode );
            else
                AddOctreeNode( onode, mOctree );
        }
    }

    void DiSceneManager::RemoveOctreeNode( DiCullNode* n)
    {
        if (!mOctree)
            return;

        DiOctreePtr oct = n->GetOctant();

        if (oct)
            oct->RemoveNode(n);

        n->SetOctant(nullptr);
    }

    void DiSceneManager::AddOctreeNode(DiCullNode* n, DiOctreePtr octant, int depth /*= 0 */)
    {
        if (!mOctree)
            return;

        const DiAABB& bx = n->GetWorldAABB();

        if ( ( depth < mMaxDepth ) && octant->IsTwiceSize( bx ) )
        {
            int id = octant->GetChildIndexes( bx );

            if (!octant->mChildren[id])
            {
                octant->mChildren[id] = make_shared<DiOctree>(octant, mOctree, id);
                const DiVec3& octantMin = octant->mBox.GetMinimum();
                const DiVec3& octantMax = octant->mBox.GetMaximum();
                
                DiVec3 min = octantMin;
                DiVec3 max = octantMax;
                if (id & 1)
                    min.x = (octantMin.x + octantMax.x) / 2;
                else
                    max.x = (octantMin.x + octantMax.x) / 2;

                if (id & 2)
                    min.y = (octantMin.y + octantMax.y) / 2;
                else
                    max.y = (octantMin.y + octantMax.y) / 2;

                if (id & 4)
                    min.z = (octantMin.z + octantMax.z) / 2;
                else
                    max.z = (octantMin.z + octantMax.z) / 2;

                octant->mChildren[id]->mBox.SetExtents( min, max );
                octant->mChildren[id]->mHalfSize = ( max - min ) / 2;
            }
            AddOctreeNode( n, octant->mChildren[id], ++depth );
        }
        else
        {
            octant->AddNode( n );
        }
    }

    void _findNodes(const DiAABB &t, DiList<DiCullNode*> &list, DiCullNode *exclude, bool full, DiOctreePtr octant)
    {

        if ( !full )
        {
            DiAABB obox;
            octant -> GetCullBounds( &obox );

            Intersection isect = intersect( t, obox );

            if ( isect == OUTSIDE )
                return ;

            full = ( isect == INSIDE );
        }

        for (DiCullNode* node = octant->mNodes.mHead; node; node = node->mNext)
        {
            if (node != exclude)
            {
                if (full)
                    list.push_back(node);
                else
                {
                    Intersection nsect = intersect(t, node->GetWorldAABB());
                    if (nsect != OUTSIDE)
                        list.push_back(node);
                }

            }
        }

        DiOctreePtr child;
        for (uint16 i = 0; i < 8; i++)
        {
            if ((child = octant->mChildren[i]))
                _findNodes(t, list, exclude, full, child);
        }
    }

    void _findNodes(const DiSphere &t, DiList< DiCullNode * >&list, DiCullNode *exclude, bool full, DiOctreePtr octant)
    {
        if ( !full )
        {
            DiAABB obox;
            octant -> GetCullBounds( &obox );

            Intersection isect = intersect( t, obox );

            if ( isect == OUTSIDE )
                return ;

            full = ( isect == INSIDE );
        }

        for (DiCullNode* node = octant->mNodes.mHead; node; node = node->mNext)
        {
            if (node != exclude)
            {
                if (full)
                    list.push_back(node);
                else
                {
                    Intersection nsect = intersect(t, node->GetWorldAABB());
                    if (nsect != OUTSIDE)
                    {
                        list.push_back(node);
                    }
                }

            }
        }

        DiOctreePtr child;
        for (uint16 i = 0; i < 8; i++)
        {
            if ((child = octant->mChildren[i]))
                _findNodes(t, list, exclude, full, child);
        }
    }


    void _findNodes( const DiPlaneBoundedVol &t, DiList< DiCullNode * > &list, 
        DiCullNode *exclude,  bool full, DiOctreePtr octant)
    {

        if ( !full )
        {
            DiAABB obox;
            octant -> GetCullBounds( &obox );

            Intersection isect = intersect( t, obox );

            if ( isect == OUTSIDE )
                return ;

            full = ( isect == INSIDE );
        }

        for (DiCullNode* node = octant->mNodes.mHead; node; node = node->mNext)
        {
            if (node != exclude)
            {
                if (full)
                    list.push_back(node);
                else
                {
                    Intersection nsect = intersect(t, node->GetWorldAABB());
                    if (nsect != OUTSIDE)
                    {
                        list.push_back(node);
                    }
                }

            }
        }

        DiOctreePtr child;
        for (uint16 i = 0; i < 8; i++)
        {
            if ((child = octant->mChildren[i]))
                _findNodes(t, list, exclude, full, child);
        }
    }

    void _findNodes(const DiRay &t, DiList< DiCullNode * >&list, DiCullNode *exclude, bool full, DiOctreePtr octant)
    {
        if ( !full )
        {
            DiAABB obox;
            octant -> GetCullBounds( &obox );

            Intersection isect = intersect( t, obox );

            if ( isect == OUTSIDE )
                return ;

            full = ( isect == INSIDE );
        }

        for (DiCullNode* node = octant->mNodes.mHead; node; node = node->mNext)
        {
            if (node != exclude)
            {
                if (full)
                    list.push_back(node);
                else
                {
                    Intersection nsect = intersect(t, node->GetWorldAABB());
                    if (nsect != OUTSIDE)
                    {
                        list.push_back(node);
                    }
                }

            }
        }

        DiOctreePtr child;
        for (uint16 i = 0; i < 8; i++)
        {
            if ((child = octant->mChildren[i]))
                _findNodes(t, list, exclude, full, child);
        }
    }

    void DiSceneManager::FindNodesIn( const DiAABB &box, DiList<DiCullNode*>& list, DiCullNode *exclude /*= 0 */ )
    {
        _findNodes( box, list, exclude, false, mOctree );
    }

    void DiSceneManager::FindNodesIn( const DiSphere &sphere, DiList<DiCullNode*>& list, DiCullNode *exclude /*= 0 */ )
    {
        _findNodes( sphere, list, exclude, false, mOctree );
    }

    void DiSceneManager::FindNodesIn( const DiPlaneBoundedVol &volume, DiList<DiCullNode*>& list, DiCullNode *exclude /*= 0 */ )
    {
        _findNodes( volume, list, exclude, false, mOctree );
    }

    void DiSceneManager::FindNodesIn( const DiRay &ray, DiList<DiCullNode*>& list, DiCullNode *exclude /*= 0 */ )
    {
        _findNodes( ray, list, exclude, false, mOctree );
    }

    DiAABBQuery* DiSceneManager::CreateAABBQuery( const DiAABB& box, unsigned long mask )
    {
        DiOcAABBQuery* q = DI_NEW DiOcAABBQuery(this);
        q->SetBox(box);
        q->SetQueryMask(mask);
        return q;
    }

    DiSphereSceneQuery* DiSceneManager::CreateSphereQuery( const DiSphere& sphere, unsigned long mask )
    {
        DiOcSphereQuery* q = DI_NEW DiOcSphereQuery(this);
        q->SetSphere(sphere);
        q->SetQueryMask(mask);
        return q;
    }

    DiPBVListSceneQuery* DiSceneManager::CreatePlaneBoundedVolumeQuery( const PlaneBoundedVolumeList& volumes, unsigned long mask )
    {
        DiOcPBVQuery* q = DI_NEW DiOcPBVQuery(this);
        q->SetVolumes(volumes);
        q->SetQueryMask(mask);
        return q;
    }

    DiRaySceneQuery* DiSceneManager::CreateRayQuery( const DiRay& ray, unsigned long mask )
    {
        DiOcRayQuery* q = DI_NEW DiOcRayQuery(this);
        q->SetRay(ray);
        q->SetQueryMask(mask);
        return q;
    }

    bool DiSceneManager::RenameSceneNode( const DiString& oldName, const DiString& newName )
    {
        auto it = mSceneNodes.find(oldName);
        if (it != mSceneNodes.end())
        {
            if (mSceneNodes.contains(newName))
            {
                return false;
            }

            DiCullNode* node = it->second;
            DI_ASSERT(node);
            node->SetName(newName);
            mSceneNodes.erase(it);
            mSceneNodes[newName] = node;
            return true;
        }
        return false;
    }

    DiCamera* DiSceneManager::CreateCamera( const DiString& name )
    {
        if (mCameras.find(name) != mCameras.end())
        {
            DI_WARNING("The camera %s has existed",name.c_str());
            return mCameras[name];
        }

        DiCamera *c = DI_NEW DiCamera(name, this);
        mCameras.insert(CameraList::value_type(name, c));

        return c;
    }

    DiCamera* DiSceneManager::GetCamera( const DiString& name ) const
    {
        auto i = mCameras.find(name);
        if (i == mCameras.end())
        {
            DI_WARNING("Cannot find the camera:%s", name.c_str());
            return nullptr;
        }
        else
        {
            return i->second;
        }
    }

    bool DiSceneManager::HasCamera( const DiString& name ) const
    {
        return (mCameras.find(name) != mCameras.end());
    }

    void DiSceneManager::DestroyCamera( DiCamera *cam )
    {
        DestroyCamera(cam->GetName());
    }

    void DiSceneManager::DestroyCamera( const DiString& name )
    {
        DI_INFO("Destroying camera : %s", name.c_str());

        auto i = mCameras.find(name);
        if (i != mCameras.end())
        {
            DI_DELETE i->second;
            mCameras.erase(i);
        }
    }

    void DiSceneManager::DestroyAllCameras( void )
    {
        auto camIt = mCameras.begin();
        while( camIt != mCameras.end() )
        {
            DestroyCamera(camIt->second);
            camIt = mCameras.begin();
        }
    }

    void DiSceneManager::AttachObject(DiTransUnitPtr obj)
    {
        mRootNode->AttachObject(obj);
    }

    void DiSceneManager::DetachObject(DiTransUnitPtr obj)
    {
        mRootNode->DetachObject(obj);
    }

    void DiSceneManager::AddExtraRenderTarget(DiRenderTarget* rt, DiCamera* camera,
        const RenderTargetParam::RTListener& preListener,
        const RenderTargetParam::RTListener& postListener)
    {
        RenderTargetParam p;
        p.rt = rt;
        p.camera = camera;
        p.preUpdateCallback = preListener;
        p.postUpdateCallback = postListener;
        mExtraRTs.push_back(p);
    }

    int     DiSceneManager::intersect_call         = 0;
    uint32  DiSceneManager::ENTITY_TYPE_MASK       = 0x80000000;
    uint32  DiSceneManager::FX_TYPE_MASK           = 0x40000000;
    uint32  DiSceneManager::LIGHT_TYPE_MASK        = 0x20000000;
    uint32  DiSceneManager::FRUSTUM_TYPE_MASK      = 0x10000000;
    uint32  DiSceneManager::USER_TYPE_MASK_LIMIT   = 0x10000000;
}
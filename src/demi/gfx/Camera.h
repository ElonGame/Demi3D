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

#ifndef DiCamera_h__
#define DiCamera_h__

#include "Frustum.h"

namespace Demi
{
    struct DI_GFX_API DiVisBoundsInfo
	{
		/// The axis-aligned bounds of the visible objects
		DiAABB aabb;
		
        /// The axis-aligned bounds of the visible shadow receiver objects
		DiAABB receiverAabb;
		
        /// The closest a visible object is to the camera
		float minDistance{ 0 };
		
        /// The farthest a visible objects is from the camera
		float maxDistance{ 0 };
		
        /// The closest a object in the frustum regardless of visibility / shadow caster flags
		float minDistanceInFrustum{ 0 };
        
		/// The farthest object in the frustum regardless of visibility / shadow caster flags
		float maxDistanceInFrustum{ 0 };
        
		DiVisBoundsInfo();
		
        void Reset();
        
		void Merge(const DiAABB& boxBounds, const DiCamera* cam, bool receiver = true);
		
		void MergeNonRenderedButInFrustum(const DiAABB& boxBounds, const DiCamera* cam);
	};
    
    /** Camera class, adapted from Ogre3D
     */
    class DI_GFX_API DiCamera : public DiFrustum
    {
    public:

        DiCamera(const DiString& name,DiSceneManager* sm);

        DiCamera(DiSceneManager* sm);

        virtual ~DiCamera(void);

        typedef DiVector<DiPlane> PlaneList;

        enum Visibility
        {
            NONE,
            PARTIAL,
            FULL
        };

    public:

        void                SetPosition(float x, float y, float z);

        void                SetPosition(const DiVec3& vec);

        const DiVec3&       GetPosition(void) const;

        void                Move(const DiVec3& vec);

        void                MoveRelative(const DiVec3& vec);

        void                SetDirection(float x, float y, float z);

        void                SetDirection(const DiVec3& vec);

        DiVec3              GetDirection(void) const;

        DiVec3              GetUp(void) const;

        DiVec3              GetRight(void) const;

        void                LookAt( const DiVec3& targetPoint );

        void                LookAt(float x, float y, float z);

        void                Roll(const DiRadian& angle);

        void                Yaw(const DiRadian& angle);

        void                Pitch(const DiRadian& angle);

        void                Rotate(const DiVec3& axis, const DiRadian& angle);

        void                Rotate(const DiQuat& q);

        void                SetFixedYawAxis( bool useFixed, const DiVec3& fixedAxis = DiVec3::UNIT_Y );

        const DiQuat&       GetOrientation(void) const;

        void                SetOrientation(const DiQuat& q);

        const DiQuat&       GetDerivedOrientation(void) const;

        const DiVec3&       GetDerivedPosition(void) const;

        DiVec3              GetDerivedDirection(void) const;

        DiVec3              GetDerivedUp(void) const;

        DiVec3              GetDerivedRight(void) const;

        const DiQuat&       GetRealOrientation(void) const;

        const DiVec3&       GetRealPosition(void) const;

        DiVec3              GetRealDirection(void) const;

        DiVec3              GetRealUp(void) const;

        DiVec3              GetRealRight(void) const;

        void                GetWorldTransforms(DiMat4* mat) const;

        DiRay               GetCameraToViewportRay(float screenx, float screeny) const;

        void                GetCameraToViewportRay(float screenx, float screeny, Demi::DiRay* outRay) const;

        DiPlaneBoundedVol   GetCameraToViewportBoxVolume(float screenLeft, 
                                    float screenTop, float screenRight, 
                                    float screenBottom, bool includeFarPlane = false);

        void                GetCameraToViewportBoxVolume(float screenLeft, 
                                    float screenTop, float screenRight, float screenBottom, 
                                    DiPlaneBoundedVol* outVolume, bool includeFarPlane = false);

        virtual void        SetWindow (float Left, float Top, float Right, float Bottom);

        virtual void        ResetWindow (void);

        virtual bool        IsWindowSet(void) const { return mWindowSet; }

        const PlaneList&    GetWindowPlanes(void) const;

        void                SetAutoAspectRatio(bool autoratio);

        bool                GetAutoAspectRatio(void) const;

        void                SetCullingFrustum(DiFrustum* frustum) { mCullFrustum = frustum; }

        DiFrustum*          GetCullingFrustum(void) const { return mCullFrustum; }

        bool                IsVisible(const DiAABB& bound, DiFrustumPlane* culledBy = 0) const;

        bool                IsVisible(const DiSphere& bound, DiFrustumPlane* culledBy = 0) const;

        bool                IsVisible(const DiVec3& vert, DiFrustumPlane* culledBy = 0) const;

        const DiVec3*       GetWorldSpaceCorners(void) const;

        const DiPlane&      GetFrustumPlane( unsigned short plane ) const;

        bool                ProjectSphere(const DiSphere& sphere, 
                                                    float* left, float* top, 
                                                    float* right, float* bottom) const;

        float               GetNearClipDistance(void) const;

        float               GetFarClipDistance(void) const;

        const DiMat4&       GetViewMatrix(void) const;

        const DiMat4&       GetViewMatrix(bool ownFrustumOnly) const;

        const DiVec3&       GetPositionForViewUpdate(void) const;

        const DiQuat&       GetOrientationForViewUpdate(void) const;

        void                SetFillMode(DiFillMode sd);

        DiFillMode          GetFillMode(void) const;

        void                CopySettingFrom(const DiCamera* cam);
    
        DiCamera*           GetSplitFrustum(float nearClip, float farClip) const;

        /** Return the parent scene manager
         */
        DiSceneManager*     GetSceneManager() const { return mSceneManager; }

        Visibility          GetVisibility( const DiAABB &bound );

        void                ForwardIntersect(const DiPlane& worldPlane, DiVector<DiVec4>* intersect3d) const;

        DiVector<DiVec4>    GetRayForwardIntersect(const DiVec3& anchor, const DiVec3 *dir, float planeOffset) const;
        
        DiVisBoundsInfo&    GetVisBoundsInfo() { return mVisBoundsInfo; }
        
        const DiVisBoundsInfo& GetVisBoundsInfo() const { return mVisBoundsInfo; }

    protected:

        DiQuat              mOrientation;

        DiVec3              mPosition;

        mutable DiQuat      mDerivedOrientation;

        mutable DiVec3      mDerivedPosition;

        mutable DiQuat      mRealOrientation;

        mutable DiVec3      mRealPosition;

        bool                mYawFixed;

        DiVec3              mYawFixedAxis;

        DiFillMode          mFillMode;

        float               mWLeft, mWTop, mWRight, mWBottom;

        bool                mWindowSet;

        mutable  PlaneList  mWindowClipPlanes;

        mutable bool        mRecalcWindow;

        bool                mAutoAspectRatio;

        DiFrustum*          mCullFrustum;

        DiSceneManager*     mSceneManager;

        bool                IsViewOutOfDate(void) const;

        void                InvalidateFrustum(void) const;

        void                InvalidateView(void) const;

        virtual void        SetWindowImpl(void) const;
        
        DiVisBoundsInfo     mVisBoundsInfo;
    };
}

#endif

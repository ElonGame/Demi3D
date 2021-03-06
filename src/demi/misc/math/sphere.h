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

#ifndef Disphere_h__
#define Disphere_h__


#include "MiscPrerequisites.h"
#include "math/vector3.h"

namespace Demi 
{
    class DiSphere
    {
    public:

        DiSphere() : mRadius(1.0), mCenter(DiVec3::ZERO) {}

        DiSphere(const DiVec3& center, float radius)
            : mRadius(radius), mCenter(center) {}

        /** Returns the radius of the sphere. */
        float getRadius(void) const { return mRadius; }

        /** Sets the radius of the sphere. */
        void setRadius(float radius) { mRadius = radius; }

        /** Returns the center point of the sphere. */
        const DiVec3& getCenter(void) const { return mCenter; }

        /** Sets the center point of the sphere. */
        void setCenter(const DiVec3& center) { mCenter = center; }

        /** Returns whether or not this sphere intersects another sphere. */
        bool intersects(const DiSphere& s) const
        {
            return (s.mCenter - mCenter).squaredLength() <=
                DiMath::Sqr(s.mRadius + mRadius);
        }
        /** Returns whether or not this sphere intersects a box. */
        bool intersects(const DiAABB& box) const
        {
            return DiMath::intersects(*this, box);
        }
        /** Returns whether or not this sphere intersects a plane. */
        bool intersects(const DiPlane& plane) const
        {
            return DiMath::intersects(*this, plane);
        }
        /** Returns whether or not this sphere intersects a point. */
        bool intersects(const DiVec3& v) const
        {
            return ((v - mCenter).squaredLength() <= DiMath::Sqr(mRadius));
        }

    protected:
        float   mRadius;
        DiVec3  mCenter;
    };

}

#endif

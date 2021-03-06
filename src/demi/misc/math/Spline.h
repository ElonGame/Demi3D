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

#ifndef DiSpline_h__
#define DiSpline_h__


namespace Demi
{
    class DI_MISC_API DiSpline
    {
    public:
        DiSpline();

        ~DiSpline();

    public:

        void                AddPoint(const DiVec3& p);

        const DiVec3&       GetPoint(unsigned short index) const;

        unsigned short      GetNumPoints(void) const;

        void                Clear(void);

        void                UpdatePoint(unsigned short index, const DiVec3& value);

        DiVec3              Interpolate(float t) const;

        DiVec3              Interpolate(unsigned int fromIndex, float t) const;

        void                SetAutoCalculate(bool autoCalc);

        void                RecalcTangents(void);

    protected:

        bool                mAutoCalc;

        DiVector<DiVec3>    mPoints;

        DiVector<DiVec3>    mTangents;

        DiMat4              mCoeffs;
    };

}

#endif

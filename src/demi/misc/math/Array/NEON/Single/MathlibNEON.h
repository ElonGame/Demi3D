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

/// This file is adapted from Ogre 2.0 (working version)

#ifndef __MathlibNEON_H__
#define __MathlibNEON_H__

#if __DEMI_HAVE_NEON

#ifndef __Mathlib_H__
    #error "Don't include this file directly. include Math/Array/Mathlib.h"
#endif

#include "MiscPrerequisites.h"

namespace Demi
{
    // A few shortcut functions to emulate instructions included with SSE2 that NEON lacks.
    // See http://rcl-rs-vvg.blogspot.com/2010/08/simd-etudes.html

    // Similar to _mm_movemask_ps
    static inline int vmovemaskq_u32( uint32x4_t conditions )
    {
        const uint32x4_t qMask = { 1, 2, 4, 8 };
        const uint32x4_t qAnded = vandq_u32( conditions, qMask );

        // These two are no-ops, they only tell compiler to treat Q register as two D regs
        const uint32x2_t dHigh = vget_high_u32( qAnded );
        const uint32x2_t dLow = vget_low_u32( qAnded );

        const uint32x2_t dOred = vorr_u32( dHigh, dLow );
        const uint32x2_t dOred2 = dOred;
        // For some reason vpadd_u32 doesn't work right if both arguments
        // are the same variable so we use a copy.
        const uint32x2_t dMask = vpadd_u32( dOred, dOred2 );
        return vget_lane_u32( dMask, 0 );
    }

    static inline ArrayInt vnand_s32(ArrayInt a, ArrayInt b)
    {
        return vandq_s32(vmvnq_s32(a), b);
    }

    static inline ArrayFloat vnand_f32(ArrayFloat a, ArrayFloat b)
    {
        return (ArrayFloat)vandq_s32(vmvnq_s32((ArrayInt)a), (ArrayInt)b);
    }

    static inline ArrayFloat vorrq_f32(ArrayFloat a, ArrayFloat b)
    {
        return (ArrayFloat)vorrq_s32((ArrayInt)a, (ArrayInt)b);
    }

    static inline ArrayFloat vdivq_f32( ArrayFloat num, ArrayFloat den )
    {
        const ArrayFloat inv0 = vrecpeq_f32(den);
        const ArrayFloat step0 = vrecpsq_f32(inv0, den);

        const ArrayFloat inv1 = vmulq_f32(step0, inv0);
        return vmulq_f32( num, inv1 );
    }

#define _MM_SHUFFLE(z, y, x, w) (((z) << 6) | ((y) << 4) | ((x) << 2) | (w))

    static inline ArrayFloat vshuf_f32(ArrayFloat a, ArrayFloat b, unsigned int idx)
    {
        float x, y, z, w;

        // First index used to retrieve from a
        switch((idx >> 6) & 0x03)
        {
            case 0:
                z = vgetq_lane_f32(a, 0);
                break;
            case 1:
                z = vgetq_lane_f32(a, 1);
                break;
            case 2:
                z = vgetq_lane_f32(a, 2);
                break;
            case 3:
                z = vgetq_lane_f32(a, 3);
                break;
        }

        // Second index used to retrieve from a
        switch((idx >> 4) & 0x03)
        {
            case 0:
                y = vgetq_lane_f32(a, 0);
                break;
            case 1:
                y = vgetq_lane_f32(a, 1);
                break;
            case 2:
                y = vgetq_lane_f32(a, 2);
                break;
            case 3:
                y = vgetq_lane_f32(a, 3);
                break;
        }

        // Third index used to retrieve from b
        switch((idx >> 2) & 0x03)
        {
            case 0:
                x = vgetq_lane_f32(b, 0);
                break;
            case 1:
                x = vgetq_lane_f32(b, 1);
                break;
            case 2:
                x = vgetq_lane_f32(b, 2);
                break;
            case 3:
                x = vgetq_lane_f32(b, 3);
                break;
        }

        // Four index used to retrieve from b
        switch(idx & 0x03)
        {
            case 0:
                w = vgetq_lane_f32(b, 0);
                break;
            case 1:
                w = vgetq_lane_f32(b, 1);
                break;
            case 2:
                w = vgetq_lane_f32(b, 2);
                break;
            case 3:
                w = vgetq_lane_f32(b, 3);
                break;
        }

        return (ArrayFloat) { x, y, z, w };
    }

    inline ArrayFloat vcneqq_f32(ArrayFloat a, ArrayFloat b)
    {
        return (ArrayFloat)vceqq_f32(vceqq_f32(a, b), vdupq_n_f32( 0.0f ));
    }

    class ArrayRadian
    {
        ArrayFloat mRad;

    public:
        explicit ArrayRadian ( ArrayFloat r ) : mRad( r ) {}
        //ArrayRadian ( const ArrayDegree& d );
        ArrayRadian& operator = ( const ArrayFloat &f )      { mRad = f; return *this; }
        ArrayRadian& operator = ( const ArrayRadian &r )    { mRad = r.mRad; return *this; }
        //ArrayRadian& operator = ( const ArrayDegree& d );

        //ArrayFloat valueDegrees() const; // see bottom of this file
        ArrayFloat valueRadians() const                      { return mRad; }

        inline const ArrayRadian& operator + () const;
        inline ArrayRadian operator + ( const ArrayRadian& r ) const;
        //inline ArrayRadian operator + ( const ArrayDegree& d ) const;
        inline ArrayRadian& operator += ( const ArrayRadian& r );
        //inline ArrayRadian& operator += ( const ArrayDegree& d );
        inline ArrayRadian operator - () const;
        inline ArrayRadian operator - ( const ArrayRadian& r ) const;
        //inline ArrayRadian operator - ( const ArrayDegree& d ) const;
        inline ArrayRadian& operator -= ( const ArrayRadian& r );
        //inline ArrayRadian& operator -= ( const ArrayDegree& d );
        inline ArrayRadian operator * ( ArrayFloat f ) const;
        inline ArrayRadian operator * ( const ArrayRadian& f ) const;
        inline ArrayRadian& operator *= ( ArrayFloat f );
        inline ArrayRadian operator / ( ArrayFloat f ) const;
        inline ArrayRadian& operator /= ( ArrayFloat f );

        inline ArrayFloat operator <  ( const ArrayRadian& r ) const;
        inline ArrayFloat operator <= ( const ArrayRadian& r ) const;
        inline ArrayFloat operator == ( const ArrayRadian& r ) const;
        inline ArrayFloat operator != ( const ArrayRadian& r ) const;
        inline ArrayFloat operator >= ( const ArrayRadian& r ) const;
        inline ArrayFloat operator >  ( const ArrayRadian& r ) const;
    };

    class DI_MISC_API MathlibNEON
    {
    public:
        static const ArrayFloat HALF;        //0.5f, 0.5f, 0.5f, 0.5f
        static const ArrayFloat ONE;         //1.0f, 1.0f, 1.0f, 1.0f
        static const ArrayFloat THREE;       //3.0f, 3.0f, 3.0f, 3.0f
        static const ArrayFloat NEG_ONE;     //-1.0f, -1.0f, -1.0f, -1.0f
        static const ArrayFloat PI;          //PI, PI, PI, PI
        static const ArrayFloat TWO_PI;      //2*PI, 2*PI, 2*PI, 2*PI
        static const ArrayFloat ONE_DIV_2PI; //1 / 2PI, 1 / 2PI, 1 / 2PI, 1 / 2PI
        static const ArrayFloat fEpsilon;    //1e-6f, 1e-6f, 1e-6f, 1e-6f
        static const ArrayFloat fSqEpsilon;  //1e-12f, 1e-12f, 1e-12f, 1e-12f
        static const ArrayFloat OneMinusEpsilon;//1 - 1e-6f, 1 - 1e-6f, 1 - 1e-6f, 1 - 1e-6f
        static const ArrayFloat fDeg2Rad;    //Math::fDeg2Rad, Math::fDeg2Rad, Math::fDeg2Rad, Math::fDeg2Rad
        static const ArrayFloat fRad2Deg;    //Math::fRad2Deg, Math::fRad2Deg, Math::fRad2Deg, Math::fRad2Deg
        static const ArrayFloat FLOAT_MIN;   //FLT_MIN, FLT_MIN, FLT_MIN, FLT_MIN
        static const ArrayFloat SIGN_MASK;   //0x80000000, 0x80000000, 0x80000000, 0x80000000
        //INFINITE is taken in Windows, INFINITY by C99 (bloody macros). A joke on Infinite Tea
        static const ArrayFloat INFINITEA;   //Inf, Inf, Inf, Inf
        static const ArrayFloat MAX_NEG;     //Max negative number (x4)
        static const ArrayFloat MAX_POS;     //Max negative number (x4)
        static const ArrayFloat LAST_AFFINE_COLUMN;//0, 0, 0, 1

        /** Returns the absolute values of each 4 floats
            @param
                4 floating point values
            @return
                abs( a )
        */
        static inline ArrayFloat Abs4( ArrayFloat a )
        {
            return vnand_f32( vdupq_n_f32( -0.0f ), a );
        }

        /** Branchless conditional move for 4 floating point values
            @remarks
                Will NOT work if any of the arguments contains Infinite
                or NaNs or non-floating point values. If an exact binary
                copy is needed, @see CmovRobust
            @param
                4 floating point values. Can't be NaN or Inf
            @param
                4 floating point values. Can't be NaN or Inf
            @param
                4 values containing either 0 or 0xffffffff
                Any other value, the result is undefined
            @return
                r[i] = mask[i] != 0 ? arg1[i] : arg2[i]
                Another way to say it:
                    if( maskCondition[i] == true )
                        r[i] = arg1[i];
                    else
                        arg2[i];
        */
        static inline ArrayFloat Cmov4( ArrayFloat arg1, ArrayFloat arg2, ArrayMaskR mask )
        {
            DI_ASSERT( vmovemaskq_u32( vceqq_f32( arg1, arg1 ) ) == 0x0f &&
                    vmovemaskq_u32( vceqq_f32( arg2, arg2 ) ) == 0x0f &&
                    "Passing NaN values to CMov4" );
#ifndef  NDEBUG
            ArrayFloat newNan1 = vmulq_f32( arg1, vdupq_n_f32(0.0f) ); //+-Inf * 0 = nan
            ArrayFloat newNan2 = vmulq_f32( arg2, vdupq_n_f32(0.0f) ); //+-Inf * 0 = nan
            assert( vmovemaskq_u32( vceqq_f32( newNan1, newNan1 ) ) == 0x0f &&
                    vmovemaskq_u32( vceqq_f32( newNan2, newNan2 ) ) == 0x0f &&
                    "Passing +/- Infinity values to CMov4" );
#endif

            ArrayFloat t = vsubq_f32( arg1, arg2 );              // t = arg1 - arg2
            return vaddq_f32( arg2, vandq_s32( t, mask ) ); // r = arg2 + (t & mask)
        }

        /** Robust, branchless conditional move for a 128-bit value.
            @remarks
                If you're looking to copy 4 floating point values that do
                not contain Inf or Nans, @see Cmov4 which is faster.
                This is because switching between registers flagged as
                floating point to integer and back has a latency delay

                For more information refer to Chapter 3.5.2.3
                Bypass between Execution Domains, Intel¨ 64 and IA-32
                Architectures Optimization Reference Manual Order
                Number: 248966-026 April (and also Table 2-12)
            @param
                A value containing 128 bits
            @param
                A value containing 128 bits
            @param
                Mask, each bit is evaluated
            @return
                For each bit:
                r[i] = mask[i] != 0 ? arg1[i] : arg2[i]
                Another way to say it:
                    if( maskCondition[i] == true )
                        r[i] = arg1[i];
                    else
                        arg2[i];
        */
        #
        static inline ArrayFloat CmovRobust( ArrayFloat arg1, ArrayFloat arg2, ArrayFloat mask )
        {
            return vorrq_f32( vandq_s32( arg1, mask ), vnand_f32( mask, arg2 ) );
        }
        static inline ArrayInt CmovRobust( ArrayInt arg1, ArrayInt arg2, ArrayMaskI mask )
        {
            return vorrq_s32( vandq_s32( arg1, mask ), vnand_s32( mask, arg2 ) );
        }

        /** Returns the result of "a & b"
        @return
            r[i] = a[i] & b[i];
        */
        static inline ArrayFloat And( ArrayFloat a, ArrayFloat b )
        {
            return vandq_s32( a, b );
        }
        static inline ArrayInt And( ArrayInt a, ArrayInt b )
        {
            return vandq_s32( a, b );
        }

        /** Returns the result of "a & b"
        @return
            r[i] = a[i] & b;
        */
        static inline ArrayInt And( ArrayInt a, uint32 b )
        {
            return vandq_s32( a, vdupq_n_s32( b ) );
        }

        /** Test if "a AND b" will result in non-zero, returning 0xffffffff on those cases
        @remarks
            Because there is no "not-equal" instruction in integer NEON, be need to do some
            bit flipping.
        @par
            Do not try to do a floating-point variation of this one. I already tried, but hit
            a wall: 0x80000000 & 0x80000000 => returns 0, because in floating point, -0 == 0
        @return
            r[i] = (a[i] & b[i]) ? 0xffffffff : 0;
        */
        static inline ArrayInt TestFlags4( ArrayInt a, ArrayInt b )
        {
            // !( (a & b) == 0 ) --> ( (a & b) == 0 ) ^ -1
            return veorq_s32( vceqq_s32( vandq_s32( a, b ), vdupq_n_s32(0) ),
                                    vdupq_n_s32( -1 ) );
        }

        /** Returns the result of "a & ~b"
        @return
            r[i] = a[i] & ~b[i];
        */
        static inline ArrayInt AndNot( ArrayInt a, ArrayInt b )
        {
            return vnand_s32( b, a );
        }

        /** Returns the result of "a | b"
        @return
            r[i] = a[i] | b[i];
        */
        static inline ArrayFloat Or( ArrayFloat a, ArrayFloat b )
        {
            return vorrq_f32( a, b );
        }
        static inline ArrayInt Or( ArrayInt a, ArrayInt b )
        {
            return vorrq_s32( a, b );
        }

        /** Returns the result of "a < b"
        @return
            r[i] = a[i] < b[i] ? 0xffffffff : 0;
        */
        static inline ArrayFloat CompareLess( ArrayFloat a, ArrayFloat b )
        {
            return vcltq_f32( a, b );
        }

        /** Returns the result of "a <= b"
         @return
         r[i] = a[i] <= b[i] ? 0xffffffff : 0;
         */
        static inline ArrayFloat CompareLessEqual( ArrayFloat a, ArrayFloat b )
        {
            return vcleq_f32( a, b );
        }

        /** Returns the result of "a > b"
        @return
            r[i] = a[i] > b[i] ? 0xffffffff : 0;
        */
        static inline ArrayFloat CompareGreater( ArrayFloat a, ArrayFloat b )
        {
            return vcgtq_f32( a, b );
        }

        /** Returns the result of "a >= b"
        @return
            r[i] = a[i] >= b[i] ? 0xffffffff : 0;
        */
        static inline ArrayMaskR CompareGreaterEqual( ArrayFloat a, ArrayFloat b )
        {
            return vcgeq_f32( a, b );
        }

        static inline ArrayFloat SetAll( float val )
        {
            return vdupq_n_f32( val );
        }

        static inline ArrayInt SetAll( uint32 val )
        {
            return vdupq_n_s32( val );
        }

        /** Returns the result of "a == std::numeric_limits<float>::infinity()"
        @return
            r[i] = a[i] == Inf ? 0xffffffff : 0;
        */
        static inline ArrayFloat isInfinity( ArrayFloat a )
        {
            return vceqq_f32( a, MathlibNEON::INFINITEA );
        }

        /// Returns the maximum value between a and b
        static inline ArrayFloat Max( ArrayFloat a, ArrayFloat b )
        {
            return vmaxq_f32( a, b );
        }

        /** Returns the minimum value of all elements in a
        @return
            r[0] = min( a[0], a[1], a[2], a[3] )
        */
        static inline float CollapseMin( ArrayFloat a )
        {
            float32x2_t a_lo, a_hi, min;
            a_lo = vget_low_f32(a);
            a_hi = vget_high_f32(a);
            min = vpmin_f32(a_lo, a_hi);
            min = vpmin_f32(min, min);

            return vget_lane_f32(min, 0);
        }

        /** Returns the maximum value of all elements in a
        @return
            r[0] = max( a[0], a[1], a[2], a[3] )
        */
        static inline float CollapseMax( ArrayFloat a )
        {
            float32x2_t a_lo, a_hi, max;
            a_lo = vget_low_f32(a);
            a_hi = vget_high_f32(a);
            max = vpmax_f32(a_lo, a_hi);
            max = vpmax_f32(max, max);

            return vget_lane_f32(max, 0);
        }

        /** Returns the reciprocal of x
            @remarks
                If you have a very rough guarantees that you won't be feeding a zero,
                consider using @see InvNonZero4 because it's faster.

                Use SSE Newton-Raphson reciprocal estimate, accurate to 23 significant
                bits of the mantissa after an extra iteration, instead of the little
                12 bits of accuracy that vrecpsq_f32 gives us 
                In short, one Newton-Raphson Iteration:
                 f( i+1 ) = 2 * rcp( f ) - f * rcp( f ) * rcp( f )
                See Intel AP-803 (Application note), Order N! 243637-002 (if you can get it!)
                 "x0 = RCPSS(d)
                x1 = x0 * (2 - d * x0) = 2 * x0 - d * x0 * x0
                where x0 is the first approximation to the reciprocal of the divisor d, and x1 is a
                better approximation. You must use this formula before multiplying with the dividend."
            @param val
                4 floating point values. If it's zero, the returned value will be infinite,
                which is the correct result, but it's slower than InvNonZero4
            @return
                1 / x (packed as 4 floats)
        */
        static inline ArrayFloat Inv4( ArrayFloat val )
        {
            ArrayFloat inv = vrecpeq_f32( val );
            ArrayFloat twoRcp    = vaddq_f32( inv, inv );                    //2 * rcp( f )
            ArrayFloat rightSide = vmulq_f32( val, vmulq_f32( inv, inv ) );  //f * rcp( f ) * rcp( f )
            rightSide = vandq_s32( rightSide, vcneqq_f32( val, vdupq_n_f32(0.0f) ) ); //Nuke this NaN
            return vsubq_f32( twoRcp, rightSide );
        }

        /** Returns the reciprocal of x
            @remarks
                If the input is zero, it will produce a NaN!!! (but it's faster)
                Note: Some architectures may slowdown when a NaN is produced, making this
                function slower than Inv4 for those cases
                @see Inv4

                Use SSE Newton-Raphson reciprocal estimate, accurate to 23 significant
                bits of the mantissa after an extra iteration, instead of the little
                12 bits of accuracy that vrecpsq_f32 gives us 
                In short, one Newton-Raphson Iteration:
                 f( i+1 ) = 2 * rcp( f ) - f * rcp( f ) * rcp( f )
                See Intel AP-803 (Application note), Order N! 243637-002 (if you can get it!)
                 "x0 = RCPSS(d)
                x1 = x0 * (2 - d * x0) = 2 * x0 - d * x0 * x0
                where x0 is the first approximation to the reciprocal of the divisor d, and x1 is a
                better approximation. You must use this formula before multiplying with the dividend."
            @param val
                4 floating point values. If it's zero, the returned value will be NaN
            @return
                1 / x (packed as 4 floats)
        */
        static inline ArrayFloat InvNonZero4( ArrayFloat val )
        {
            ArrayFloat inv = vrecpeq_f32( val );
            ArrayFloat twoRcp    = vaddq_f32( inv, inv );                    //2 * rcp( f )
            ArrayFloat rightSide= vmulq_f32( val, vmulq_f32( inv, inv ) );   //f * rcp( f ) * rcp( f )
            return vsubq_f32( twoRcp, rightSide );
        }

        /** Returns the squared root of the reciprocal of x
            @remarks
                Use SSE Newton-Raphson reciprocal estimate, accurate to 23 significant
                bits of the mantissa after an extra iteration, instead of the little
                12 bits of accuracy that vrecpsq_f32 gives us 
                In short, one Newton-Raphson Iteration:
                     f( i+1 ) = 0.5 * rsqrt( f ) * ( 3 - f * rsqrt( f ) * rsqrt( f ) )
                See Intel AP-803 (Application note), Order N! 243637-002 (if you can get it!)
                "x0 = RSQRTSS(a)
                x1 = 0.5 * x0 * ( 3 - ( a * x0 ) * x0 )
                where x0 is the first approximation to the reciprocal square root of a, and x1 is a
                better approximation. The order of evaluation is important. You must use this formula
                before multiplying with a to get the square root."
            @param f
                4 floating point values
            @return
                1 / sqrt( x ) (packed as 4 floats)
        */
        static inline ArrayFloat InvSqrt4( ArrayFloat f )
        {
            ArrayFloat invSqrt   = vrsqrteq_f32( f );

            ArrayFloat halfInvSqrt= vmulq_f32( HALF, invSqrt );                      //0.5 * rsqrt( f )
            ArrayFloat rightSide  = vmulq_f32( invSqrt, vmulq_f32( f, invSqrt ) );   //f * rsqrt( f ) * rsqrt( f )
            rightSide = vandq_s32( rightSide, vcneqq_f32( f, vdupq_n_f32(0.0f) ) );//Nuke this NaN
            return vmulq_f32( halfInvSqrt, vsubq_f32( THREE, rightSide ) );     //halfInvSqrt*(3 - rightSide)
        }

        /** Returns the squared root of the reciprocal of x
            @remarks
                Use SSE Newton-Raphson reciprocal estimate, accurate to 23 significant
                bits of the mantissa after an extra iteration, instead of the little
                12 bits of accuracy that vrecpsq_f32 gives us
                In short, one Newton-Raphson Iteration:
                     f( i+1 ) = 0.5 * rsqrt( f ) * ( 3 - f * rsqrt( f ) * rsqrt( f ) )
                See Intel AP-803 (Application note), Order N! 243637-002 (if you can get it!)
                "x0 = RSQRTSS(a)
                x1 = 0.5 * x0 * ( 3 - ( a * x0 ) * x0 )
                where x0 is the first approximation to the reciprocal square root of a, and x1 is a
                better approximation. The order of evaluation is important. You must use this formula
                before multiplying with a to get the square root."

                Warning: Passing a zero will return a NaN instead of infinity
            @param f
                4 floating point values
            @return
                1 / sqrt( x ) (packed as 4 floats)
        */
        static inline ArrayFloat InvSqrtNonZero4( ArrayFloat f )
        {
            ArrayFloat invSqrt = vrsqrteq_f32( f );

            ArrayFloat halfInvSqrt= vmulq_f32( HALF, invSqrt );                      //0.5 * rsqrt( f )
            ArrayFloat rightSide  = vmulq_f32( invSqrt, vmulq_f32( f, invSqrt ) );   //f * rsqrt( f ) * rsqrt( f )
            return vmulq_f32( halfInvSqrt, vsubq_f32( THREE, rightSide ) );     //halfInvSqrt*(3 - rightSide)
        }

        /** Break x into fractional and integral parts
            @param x
                4 floating point values. i.e. "2.57" (x4)
            @param outIntegral
                The integral part of x. i.e. 2
            @return
                The fractional part of x. i.e. 0.57
        */
        static inline ArrayFloat Modf4( ArrayFloat x, ArrayFloat &outIntegral );

        /** Returns the arccos of x
            @param x
                4 floating point values
            @return
                arccos( x ) (packed as 4 floats)
        */
        static inline ArrayFloat ACos4( ArrayFloat x );

        /** Returns the sine of x
            @param x
                4 floating point values
            @return
                sin( x ) (packed as 4 floats)
        */
        static ArrayFloat Sin4( ArrayFloat x );

        /** Returns the cosine of x
            @param x
                4 floating point values
            @return
                cos( x ) (packed as 4 floats)
        */
        static ArrayFloat Cos4( ArrayFloat x );

        /** Calculates the cosine & sine of x. Use this function if you need to calculate
            both, as it is faster than calling Cos4 & Sin4 together.
            @param x
                4 floating point values
            @param outSin
                Output value, sin( x ) (packed as 4 floats)
            @param outCos
                Output value, cos( x ) (packed as 4 floats)
        */
        static void SinCos4( ArrayFloat x, ArrayFloat &outSin, ArrayFloat &outCos );
    };

#if DEMI_COMPILER != DEMI_COMPILER_CLANG && DEMI_COMPILER != DEMI_COMPILER_GNUC
//  inline ArrayFloat operator - ( ArrayFloat l )                 { return _mm_xor_ps( l, MathlibNEON::SIGN_MASK ); }
//  inline ArrayFloat operator + ( ArrayFloat l, float r )         { return vaddq_f32( l, vdupq_n_f32( r ) ); }
//  inline ArrayFloat operator + ( float l, ArrayFloat r )         { return vaddq_f32( vdupq_n_f32( l ), r ); }
//  inline ArrayFloat operator + ( ArrayFloat l, ArrayFloat r )    { return vaddq_f32( l, r ); }
//  inline ArrayFloat operator - ( ArrayFloat l, float r )         { return vsubq_f32( l, vdupq_n_f32( r ) ); }
//  inline ArrayFloat operator - ( float l, ArrayFloat r )         { return vsubq_f32( vdupq_n_f32( l ), r ); }
    inline ArrayFloat operator - ( ArrayFloat l, ArrayFloat r )    { return vsubq_f32( l, r ); }
//  inline ArrayFloat operator * ( ArrayFloat l, float r )         { return vmulq_f32( l, vdupq_n_f32( r ) ); }
//  inline ArrayFloat operator * ( float l, ArrayFloat r )         { return vmulq_f32( vdupq_n_f32( l ), r ); }
    inline ArrayFloat operator * ( ArrayFloat l, ArrayFloat r )    { return vmulq_f32( l, r ); }
//  inline ArrayFloat operator / ( ArrayFloat l, float r )         { return _mm_div_ps( l, vdupq_n_f32( r ) ); }
//  inline ArrayFloat operator / ( float l, ArrayFloat r )         { return _mm_div_ps( vdupq_n_f32( l ), r ); }
//  inline ArrayFloat operator / ( ArrayFloat l, ArrayFloat r )    { return _mm_div_ps( l, r ); }
#endif
}

#include "MathlibNEON.inl"

#endif
#endif

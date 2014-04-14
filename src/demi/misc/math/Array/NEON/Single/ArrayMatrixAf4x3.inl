
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

namespace Demi
{
    /// Concatenates two 4x3 array matrices.
    /// @remarks
    ///     99.99% of the cases the matrix isn't concatenated with itself, therefore it's
    ///     safe to assume &lhs != &rhs. RESTRICT_ALIAS modifier is used (a non-standard
    ///     C++ extension) is used when available to dramatically improve performance,
    ///     particularly of the update operations ( a *= b )
    ///     This function will assert if DEMI_RESTRICT_ALIASING is enabled and any of the
    ///     given pointers point to the same location
    FORCEINLINE void concatArrayMatAf4x3( ArrayFloat * RESTRICT_ALIAS outChunkBase,
                                        const ArrayFloat * RESTRICT_ALIAS lhsChunkBase,
                                        const ArrayFloat * RESTRICT_ALIAS rhsChunkBase )
    {
#if DEMI_RESTRICT_ALIASING != 0
        assert( outChunkBase != lhsChunkBase && outChunkBase != rhsChunkBase &&
                lhsChunkBase != rhsChunkBase &&
                "Re-strict aliasing rule broken. Compile without DEMI_RESTRICT_ALIASING" );
#endif
        outChunkBase[0] =
            vaddq_f32(
            vaddq_f32(
                vmulq_f32( lhsChunkBase[0], rhsChunkBase[0] ),
                vmulq_f32( lhsChunkBase[1], rhsChunkBase[4] ) ),
            vaddq_f32(
                vmulq_f32( lhsChunkBase[2], rhsChunkBase[8] ),
                vmulq_f32( lhsChunkBase[3], rhsChunkBase[12] ) ) );
        outChunkBase[1] =
            vaddq_f32(
            vaddq_f32(
                vmulq_f32( lhsChunkBase[0], rhsChunkBase[1] ),
                vmulq_f32( lhsChunkBase[1], rhsChunkBase[5] ) ),
            vaddq_f32(
                vmulq_f32( lhsChunkBase[2], rhsChunkBase[9] ),
                vmulq_f32( lhsChunkBase[3], rhsChunkBase[13] ) ) );
        outChunkBase[2] =
            vaddq_f32(
            vaddq_f32(
                vmulq_f32( lhsChunkBase[0], rhsChunkBase[2] ),
                vmulq_f32( lhsChunkBase[1], rhsChunkBase[6] ) ),
            vaddq_f32(
                vmulq_f32( lhsChunkBase[2], rhsChunkBase[10] ),
                vmulq_f32( lhsChunkBase[3], rhsChunkBase[14] ) ) );
        outChunkBase[3] =
            vaddq_f32(
            vaddq_f32(
                vmulq_f32( lhsChunkBase[0], rhsChunkBase[3] ),
                vmulq_f32( lhsChunkBase[1], rhsChunkBase[7] ) ),
            vaddq_f32(
                vmulq_f32( lhsChunkBase[2], rhsChunkBase[11] ),
                vmulq_f32( lhsChunkBase[3], rhsChunkBase[15] ) ) );

        /* Next row (1) */
        outChunkBase[4] =
            vaddq_f32(
            vaddq_f32(
                vmulq_f32( lhsChunkBase[4], rhsChunkBase[0] ),
                vmulq_f32( lhsChunkBase[5], rhsChunkBase[4] ) ),
            vaddq_f32(
                vmulq_f32( lhsChunkBase[6], rhsChunkBase[8] ),
                vmulq_f32( lhsChunkBase[7], rhsChunkBase[12] ) ) );
        outChunkBase[5] =
            vaddq_f32(
            vaddq_f32(
                vmulq_f32( lhsChunkBase[4], rhsChunkBase[1] ),
                vmulq_f32( lhsChunkBase[5], rhsChunkBase[5] ) ),
            vaddq_f32(
                vmulq_f32( lhsChunkBase[6], rhsChunkBase[9] ),
                vmulq_f32( lhsChunkBase[7], rhsChunkBase[13] ) ) );
        outChunkBase[6] =
            vaddq_f32(
            vaddq_f32(
                vmulq_f32( lhsChunkBase[4], rhsChunkBase[2] ),
                vmulq_f32( lhsChunkBase[5], rhsChunkBase[6] ) ),
            vaddq_f32(
                vmulq_f32( lhsChunkBase[6], rhsChunkBase[10] ),
                vmulq_f32( lhsChunkBase[7], rhsChunkBase[14] ) ) );
        outChunkBase[7] =
            vaddq_f32(
            vaddq_f32(
                vmulq_f32( lhsChunkBase[4], rhsChunkBase[3] ),
                vmulq_f32( lhsChunkBase[5], rhsChunkBase[7] ) ),
            vaddq_f32(
                vmulq_f32( lhsChunkBase[6], rhsChunkBase[11] ),
                vmulq_f32( lhsChunkBase[7], rhsChunkBase[15] ) ) );

        /* Next row (2) */
        outChunkBase[8] =
            vaddq_f32(
            vaddq_f32(
                vmulq_f32( lhsChunkBase[8], rhsChunkBase[0] ),
                vmulq_f32( lhsChunkBase[9], rhsChunkBase[4] ) ),
            vaddq_f32(
                vmulq_f32( lhsChunkBase[10], rhsChunkBase[8] ),
                vmulq_f32( lhsChunkBase[11], rhsChunkBase[12] ) ) );
        outChunkBase[9] =
            vaddq_f32(
            vaddq_f32(
                vmulq_f32( lhsChunkBase[8], rhsChunkBase[1] ),
                vmulq_f32( lhsChunkBase[9], rhsChunkBase[5] ) ),
            vaddq_f32(
                vmulq_f32( lhsChunkBase[10], rhsChunkBase[9] ),
                vmulq_f32( lhsChunkBase[11], rhsChunkBase[13] ) ) );
        outChunkBase[10] =
            vaddq_f32(
            vaddq_f32(
                vmulq_f32( lhsChunkBase[8], rhsChunkBase[2] ),
                vmulq_f32( lhsChunkBase[9], rhsChunkBase[6] ) ),
            vaddq_f32(
                vmulq_f32( lhsChunkBase[10], rhsChunkBase[10] ),
                vmulq_f32( lhsChunkBase[11], rhsChunkBase[14] ) ) );
        outChunkBase[11] =
            vaddq_f32(
            vaddq_f32(
                vmulq_f32( lhsChunkBase[8], rhsChunkBase[3] ),
                vmulq_f32( lhsChunkBase[9], rhsChunkBase[7] ) ),
            vaddq_f32(
                vmulq_f32( lhsChunkBase[10], rhsChunkBase[11] ),
                vmulq_f32( lhsChunkBase[11], rhsChunkBase[15] ) ) );
    }

    /// Update version
    FORCEINLINE void concatArrayMatAf4x3( ArrayFloat * RESTRICT_ALIAS lhsChunkBase,
                                        const ArrayFloat * RESTRICT_ALIAS rhsChunkBase )
    {
#if DEMI_RESTRICT_ALIASING != 0
        assert( lhsChunkBase != rhsChunkBase &&
                "Re-strict aliasing rule broken. Compile without DEMI_RESTRICT_ALIASING" );
#endif
        ArrayFloat lhs0 = lhsChunkBase[0];
        lhsChunkBase[0] =
            vaddq_f32(
            vaddq_f32(
                vmulq_f32( lhsChunkBase[0], rhsChunkBase[0] ),
                vmulq_f32( lhsChunkBase[1], rhsChunkBase[4] ) ),
            vaddq_f32(
                vmulq_f32( lhsChunkBase[2], rhsChunkBase[8] ),
                vmulq_f32( lhsChunkBase[3], rhsChunkBase[12] ) ) );
        ArrayFloat lhs1 = lhsChunkBase[1];
        lhsChunkBase[1] =
            vaddq_f32(
            vaddq_f32(
                vmulq_f32( lhs0, rhsChunkBase[1] ),
                vmulq_f32( lhsChunkBase[1], rhsChunkBase[5] ) ),
            vaddq_f32(
                vmulq_f32( lhsChunkBase[2], rhsChunkBase[9] ),
                vmulq_f32( lhsChunkBase[3], rhsChunkBase[13] ) ) );
        ArrayFloat lhs2 = lhsChunkBase[2];
        lhsChunkBase[2] =
            vaddq_f32(
            vaddq_f32(
                vmulq_f32( lhs0, rhsChunkBase[2] ),
                vmulq_f32( lhs1, rhsChunkBase[6] ) ),
            vaddq_f32(
                vmulq_f32( lhsChunkBase[2], rhsChunkBase[10] ),
                vmulq_f32( lhsChunkBase[3], rhsChunkBase[14] ) ) );
        lhsChunkBase[3] =
            vaddq_f32(
            vaddq_f32(
                vmulq_f32( lhs0, rhsChunkBase[3] ),
                vmulq_f32( lhs1, rhsChunkBase[7] ) ),
            vaddq_f32(
                vmulq_f32( lhs2, rhsChunkBase[11] ),
                vmulq_f32( lhsChunkBase[3], rhsChunkBase[15] ) ) );

        /* Next row (1) */
        lhs0 = lhsChunkBase[4];
        lhsChunkBase[4] =
            vaddq_f32(
            vaddq_f32(
                vmulq_f32( lhsChunkBase[4], rhsChunkBase[0] ),
                vmulq_f32( lhsChunkBase[5], rhsChunkBase[4] ) ),
            vaddq_f32(
                vmulq_f32( lhsChunkBase[6], rhsChunkBase[8] ),
                vmulq_f32( lhsChunkBase[7], rhsChunkBase[12] ) ) );
        lhs1 = lhsChunkBase[5];
        lhsChunkBase[5] =
            vaddq_f32(
            vaddq_f32(
                vmulq_f32( lhs0, rhsChunkBase[1] ),
                vmulq_f32( lhsChunkBase[5], rhsChunkBase[5] ) ),
            vaddq_f32(
                vmulq_f32( lhsChunkBase[6], rhsChunkBase[9] ),
                vmulq_f32( lhsChunkBase[7], rhsChunkBase[13] ) ) );
        lhs2 = lhsChunkBase[6];
        lhsChunkBase[6] =
            vaddq_f32(
            vaddq_f32(
                vmulq_f32( lhs0, rhsChunkBase[2] ),
                vmulq_f32( lhs1, rhsChunkBase[6] ) ),
            vaddq_f32(
                vmulq_f32( lhsChunkBase[6], rhsChunkBase[10] ),
                vmulq_f32( lhsChunkBase[7], rhsChunkBase[14] ) ) );
        lhsChunkBase[7] =
            vaddq_f32(
            vaddq_f32(
                vmulq_f32( lhs0, rhsChunkBase[3] ),
                vmulq_f32( lhs1, rhsChunkBase[7] ) ),
            vaddq_f32(
                vmulq_f32( lhs2, rhsChunkBase[11] ),
                vmulq_f32( lhsChunkBase[7], rhsChunkBase[15] ) ) );

        /* Next row (2) */
        lhs0 = lhsChunkBase[8];
        lhsChunkBase[8] =
            vaddq_f32(
            vaddq_f32(
                vmulq_f32( lhsChunkBase[8], rhsChunkBase[0] ),
                vmulq_f32( lhsChunkBase[9], rhsChunkBase[4] ) ),
            vaddq_f32(
                vmulq_f32( lhsChunkBase[10], rhsChunkBase[8] ),
                vmulq_f32( lhsChunkBase[11], rhsChunkBase[12] ) ) );
        lhs1 = lhsChunkBase[9];
        lhsChunkBase[9] =
            vaddq_f32(
            vaddq_f32(
                vmulq_f32( lhs0, rhsChunkBase[1] ),
                vmulq_f32( lhsChunkBase[9], rhsChunkBase[5] ) ),
            vaddq_f32(
                vmulq_f32( lhsChunkBase[10], rhsChunkBase[9] ),
                vmulq_f32( lhsChunkBase[11], rhsChunkBase[13] ) ) );
        lhs2 = lhsChunkBase[10];
        lhsChunkBase[10] =
            vaddq_f32(
            vaddq_f32(
                vmulq_f32( lhs0, rhsChunkBase[2] ),
                vmulq_f32( lhs1, rhsChunkBase[6] ) ),
            vaddq_f32(
                vmulq_f32( lhsChunkBase[10], rhsChunkBase[10] ),
                vmulq_f32( lhsChunkBase[11], rhsChunkBase[14] ) ) );
        lhsChunkBase[11] =
            vaddq_f32(
            vaddq_f32(
                vmulq_f32( lhs0, rhsChunkBase[3] ),
                vmulq_f32( lhs1, rhsChunkBase[7] ) ),
            vaddq_f32(
                vmulq_f32( lhs2, rhsChunkBase[11] ),
                vmulq_f32( lhsChunkBase[11], rhsChunkBase[15] ) ) );
    }

    FORCEINLINE ArrayMatrixAf4x3 operator * ( const ArrayMatrixAf4x3 &lhs, const ArrayMatrixAf4x3 &rhs )
    {
        ArrayMatrixAf4x3 retVal;
        concatArrayMatAf4x3( retVal.mChunkBase, lhs.mChunkBase, rhs.mChunkBase );
        return retVal;
    }
    
    inline ArrayVector3 ArrayMatrixAf4x3::operator * ( const ArrayVector3 &rhs ) const
    {
        return ArrayVector3(
            //X
            //  (m00 * v.x + m01 * v.y) + (m02 * v.z + m03)
            vaddq_f32(
                _mm_madd_ps( mChunkBase[0], rhs.mChunkBase[0],
                vmulq_f32( mChunkBase[1], rhs.mChunkBase[1] ) ),
                _mm_madd_ps( mChunkBase[2], rhs.mChunkBase[2],
                            mChunkBase[3] ) ),
            //Y
            //  (m10 * v.x + m11 * v.y) + (m12 * v.z + m13)
            vaddq_f32(
                _mm_madd_ps( mChunkBase[4], rhs.mChunkBase[0],
                vmulq_f32( mChunkBase[5], rhs.mChunkBase[1] ) ),
                _mm_madd_ps( mChunkBase[6], rhs.mChunkBase[2],
                            mChunkBase[7] ) ),
            //Z
            //  (m20 * v.x + m21 * v.y) + (m22 * v.z + m23)
            vaddq_f32(
                _mm_madd_ps( mChunkBase[8], rhs.mChunkBase[0],
                vmulq_f32( mChunkBase[9], rhs.mChunkBase[1] ) ),
                _mm_madd_ps( mChunkBase[10], rhs.mChunkBase[2],
                            mChunkBase[11] ) ) );
    }
    
    FORCEINLINE void ArrayMatrixAf4x3::operator *= ( const ArrayMatrixAf4x3 &rhs )
    {
        concatArrayMatAf4x3( mChunkBase, rhs.mChunkBase );
    }
    
    inline void ArrayMatrixAf4x3::fromQuaternion( const ArrayQuaternion &q )
    {
        ArrayFloat * RESTRICT_ALIAS chunkBase = mChunkBase;
        const ArrayFloat * RESTRICT_ALIAS qChunkBase = q.mChunkBase;
        ArrayFloat fTx  = vaddq_f32( qChunkBase[1], qChunkBase[1] );     // 2 * x
        ArrayFloat fTy  = vaddq_f32( qChunkBase[2], qChunkBase[2] );     // 2 * y
        ArrayFloat fTz  = vaddq_f32( qChunkBase[3], qChunkBase[3] );     // 2 * z
        ArrayFloat fTwx = vmulq_f32( fTx, qChunkBase[0] );                   // fTx*w;
        ArrayFloat fTwy = vmulq_f32( fTy, qChunkBase[0] );                   // fTy*w;
        ArrayFloat fTwz = vmulq_f32( fTz, qChunkBase[0] );                   // fTz*w;
        ArrayFloat fTxx = vmulq_f32( fTx, qChunkBase[1] );                   // fTx*x;
        ArrayFloat fTxy = vmulq_f32( fTy, qChunkBase[1] );                   // fTy*x;
        ArrayFloat fTxz = vmulq_f32( fTz, qChunkBase[1] );                   // fTz*x;
        ArrayFloat fTyy = vmulq_f32( fTy, qChunkBase[2] );                   // fTy*y;
        ArrayFloat fTyz = vmulq_f32( fTz, qChunkBase[2] );                   // fTz*y;
        ArrayFloat fTzz = vmulq_f32( fTz, qChunkBase[3] );                   // fTz*z;

        chunkBase[0] = vsubq_f32( MathlibNEON::ONE, vaddq_f32( fTyy, fTzz ) );
        chunkBase[1] = vsubq_f32( fTxy, fTwz );
        chunkBase[2] = vaddq_f32( fTxz, fTwy );
        chunkBase[4] = vaddq_f32( fTxy, fTwz );
        chunkBase[5] = vsubq_f32( MathlibNEON::ONE, vaddq_f32( fTxx, fTzz ) );
        chunkBase[6] = vsubq_f32( fTyz, fTwx );
        chunkBase[8] = vsubq_f32( fTxz, fTwy );
        chunkBase[9] = vaddq_f32( fTyz, fTwx );
        chunkBase[10]= vsubq_f32( MathlibNEON::ONE, vaddq_f32( fTxx, fTyy ) );
    }
    
    inline void ArrayMatrixAf4x3::makeTransform( const ArrayVector3 &position, const ArrayVector3 &scale,
                                             const ArrayQuaternion &orientation )
    {
        ArrayFloat * RESTRICT_ALIAS chunkBase            = mChunkBase;
        const ArrayFloat * RESTRICT_ALIAS posChunkBase   = position.mChunkBase;
        const ArrayFloat * RESTRICT_ALIAS scaleChunkBase = scale.mChunkBase;
        this->fromQuaternion( orientation );
        chunkBase[0] = vmulq_f32( chunkBase[0], scaleChunkBase[0] );    //m00 * scale.x
        chunkBase[1] = vmulq_f32( chunkBase[1], scaleChunkBase[1] );    //m01 * scale.y
        chunkBase[2] = vmulq_f32( chunkBase[2], scaleChunkBase[2] );    //m02 * scale.z
        chunkBase[3] =  posChunkBase[0];                                //m03 * pos.x

        chunkBase[4] = vmulq_f32( chunkBase[4], scaleChunkBase[0] );    //m10 * scale.x
        chunkBase[5] = vmulq_f32( chunkBase[5], scaleChunkBase[1] );    //m11 * scale.y
        chunkBase[6] = vmulq_f32( chunkBase[6], scaleChunkBase[2] );    //m12 * scale.z
        chunkBase[7] =  posChunkBase[1];                                //m13 * pos.y

        chunkBase[8] = vmulq_f32( chunkBase[8], scaleChunkBase[0] );    //m20 * scale.x
        chunkBase[9] = vmulq_f32( chunkBase[9], scaleChunkBase[1] );    //m21 * scale.y
        chunkBase[10]= vmulq_f32( chunkBase[10],scaleChunkBase[2] );    //m22 * scale.z
        chunkBase[11]=  posChunkBase[2];                                //m23 * pos.z
    }
    
    inline void ArrayMatrixAf4x3::setToInverse(void)
    {
        ArrayFloat m10 = mChunkBase[4], m11 = mChunkBase[5], m12 = mChunkBase[6];
        ArrayFloat m20 = mChunkBase[8], m21 = mChunkBase[9], m22 = mChunkBase[10];

        ArrayFloat t00 = _mm_nmsub_ps( m21, m12, vmulq_f32( m22, m11 ) ); //m22 * m11 - m21 * m12;
        ArrayFloat t10 = _mm_nmsub_ps( m22, m10, vmulq_f32( m20, m12 ) ); //m20 * m22 - m22 * m10;
        ArrayFloat t20 = _mm_nmsub_ps( m20, m11, vmulq_f32( m21, m10 ) ); //m21 * m10 - m20 * m11;

        ArrayFloat m00 = mChunkBase[0], m01 = mChunkBase[1], m02 = mChunkBase[2];

        //det = m00 * t00 + m01 * t10 + m02 * t20
        ArrayFloat det   = _mm_madd_ps( m00, t00, _mm_madd_ps( m01, t10,  vmulq_f32( m02, t20 ) ) );
        ArrayFloat invDet= vdivq_f32( MathlibNEON::ONE, det ); //High precision division

        t00 = vmulq_f32( t00, invDet );
        t10 = vmulq_f32( t10, invDet );
        t20 = vmulq_f32( t20, invDet );

        m00 = vmulq_f32( m00, invDet );
        m01 = vmulq_f32( m01, invDet );
        m02 = vmulq_f32( m02, invDet );

        ArrayFloat r00 = t00;
        ArrayFloat r01 = _mm_nmsub_ps( m01, m22, vmulq_f32( m02, m21 ) ); //m02 * m21 - m01 * m22;
        ArrayFloat r02 = _mm_nmsub_ps( m02, m11, vmulq_f32( m01, m12 ) ); //m01 * m12 - m02 * m11;

        ArrayFloat r10 = t10;
        ArrayFloat r11 = _mm_nmsub_ps( m02, m20, vmulq_f32( m00, m22 ) ); //m00 * m22 - m02 * m20;
        ArrayFloat r12 = _mm_nmsub_ps( m00, m12, vmulq_f32( m02, m10 ) ); //m02 * m10 - m00 * m12;

        ArrayFloat r20 = t20;
        ArrayFloat r21 = _mm_nmsub_ps( m00, m21, vmulq_f32( m01, m20 ) ); //m01 * m20 - m00 * m21;
        ArrayFloat r22 = _mm_nmsub_ps( m01, m10, vmulq_f32( m00, m11 ) ); //m00 * m11 - m01 * m10;

        ArrayFloat m03 = mChunkBase[3], m13 = mChunkBase[7], m23 = mChunkBase[11];

        //r03 = (r00 * m03 + r01 * m13 + r02 * m23)
        //r13 = (r10 * m03 + r11 * m13 + r12 * m23)
        //r23 = (r20 * m03 + r21 * m13 + r22 * m23)
        ArrayFloat r03 = _mm_madd_ps( r00, m03, _mm_madd_ps( r01, m13, vmulq_f32( r02, m23 ) ) );
        ArrayFloat r13 = _mm_madd_ps( r10, m03, _mm_madd_ps( r11, m13, vmulq_f32( r12, m23 ) ) );
        ArrayFloat r23 = _mm_madd_ps( r20, m03, _mm_madd_ps( r21, m13, vmulq_f32( r22, m23 ) ) );

        r03 = vmulq_f32( r03, MathlibNEON::NEG_ONE ); //r03 = -r03
        r13 = vmulq_f32( r13, MathlibNEON::NEG_ONE ); //r13 = -r13
        r23 = vmulq_f32( r23, MathlibNEON::NEG_ONE ); //r23 = -r23

        mChunkBase[0] = r00;
        mChunkBase[1] = r01;
        mChunkBase[2] = r02;
        mChunkBase[3] = r03;

        mChunkBase[4] = r10;
        mChunkBase[5] = r11;
        mChunkBase[6] = r12;
        mChunkBase[7] = r13;

        mChunkBase[8] = r20;
        mChunkBase[9] = r21;
        mChunkBase[10]= r22;
        mChunkBase[11]= r23;
    }
    
    inline void ArrayMatrixAf4x3::setToInverseDegeneratesAsIdentity(void)
    {
        ArrayFloat m10 = mChunkBase[4], m11 = mChunkBase[5], m12 = mChunkBase[6];
        ArrayFloat m20 = mChunkBase[8], m21 = mChunkBase[9], m22 = mChunkBase[10];

        ArrayFloat t00 = _mm_nmsub_ps( m21, m12, vmulq_f32( m22, m11 ) ); //m22 * m11 - m21 * m12;
        ArrayFloat t10 = _mm_nmsub_ps( m22, m10, vmulq_f32( m20, m12 ) ); //m20 * m22 - m22 * m10;
        ArrayFloat t20 = _mm_nmsub_ps( m20, m11, vmulq_f32( m21, m10 ) ); //m21 * m10 - m20 * m11;

        ArrayFloat m00 = mChunkBase[0], m01 = mChunkBase[1], m02 = mChunkBase[2];

        //det = m00 * t00 + m01 * t10 + m02 * t20
        ArrayFloat det   = _mm_madd_ps( m00, t00, _mm_madd_ps( m01, t10,  vmulq_f32( m02, t20 ) ) );
        ArrayFloat invDet= vdivq_f32( MathlibNEON::ONE, det ); //High precision division

        //degenerateMask = Abs( det ) < fEpsilon;
        ArrayMaskR degenerateMask = vcltq_f32( MathlibNEON::Abs4( det ), MathlibNEON::fEpsilon );

        t00 = vmulq_f32( t00, invDet );
        t10 = vmulq_f32( t10, invDet );
        t20 = vmulq_f32( t20, invDet );

        m00 = vmulq_f32( m00, invDet );
        m01 = vmulq_f32( m01, invDet );
        m02 = vmulq_f32( m02, invDet );

        ArrayFloat r00 = t00;
        ArrayFloat r01 = _mm_nmsub_ps( m01, m22, vmulq_f32( m02, m21 ) ); //m02 * m21 - m01 * m22;
        ArrayFloat r02 = _mm_nmsub_ps( m02, m11, vmulq_f32( m01, m12 ) ); //m01 * m12 - m02 * m11;

        ArrayFloat r10 = t10;
        ArrayFloat r11 = _mm_nmsub_ps( m02, m20, vmulq_f32( m00, m22 ) ); //m00 * m22 - m02 * m20;
        ArrayFloat r12 = _mm_nmsub_ps( m00, m12, vmulq_f32( m02, m10 ) ); //m02 * m10 - m00 * m12;

        ArrayFloat r20 = t20;
        ArrayFloat r21 = _mm_nmsub_ps( m00, m21, vmulq_f32( m01, m20 ) ); //m01 * m20 - m00 * m21;
        ArrayFloat r22 = _mm_nmsub_ps( m01, m10, vmulq_f32( m00, m11 ) ); //m00 * m11 - m01 * m10;

        ArrayFloat m03 = mChunkBase[3], m13 = mChunkBase[7], m23 = mChunkBase[11];

        //r03 = (r00 * m03 + r01 * m13 + r02 * m23)
        //r13 = (r10 * m03 + r11 * m13 + r12 * m23)
        //r13 = (r20 * m03 + r21 * m13 + r22 * m23)
        ArrayFloat r03 = _mm_madd_ps( r00, m03, _mm_madd_ps( r01, m13, vmulq_f32( r02, m23 ) ) );
        ArrayFloat r13 = _mm_madd_ps( r10, m03, _mm_madd_ps( r11, m13, vmulq_f32( r12, m23 ) ) );
        ArrayFloat r23 = _mm_madd_ps( r20, m03, _mm_madd_ps( r21, m13, vmulq_f32( r22, m23 ) ) );

        r03 = vmulq_f32( r03, MathlibNEON::NEG_ONE ); //r03 = -r03
        r13 = vmulq_f32( r13, MathlibNEON::NEG_ONE ); //r13 = -r13
        r23 = vmulq_f32( r23, MathlibNEON::NEG_ONE ); //r23 = -r23

        mChunkBase[0] = MathlibNEON::CmovRobust( MathlibNEON::ONE, r00, degenerateMask );
        mChunkBase[1] = MathlibNEON::CmovRobust( vdupq_n_f32( 0.0f ), r01, degenerateMask );
        mChunkBase[2] = MathlibNEON::CmovRobust( vdupq_n_f32( 0.0f ), r02, degenerateMask );
        mChunkBase[3] = MathlibNEON::CmovRobust( vdupq_n_f32( 0.0f ), r03, degenerateMask );

        mChunkBase[4] = MathlibNEON::CmovRobust( vdupq_n_f32( 0.0f ), r10, degenerateMask );
        mChunkBase[5] = MathlibNEON::CmovRobust( MathlibNEON::ONE, r11, degenerateMask );
        mChunkBase[6] = MathlibNEON::CmovRobust( vdupq_n_f32( 0.0f ), r12, degenerateMask );
        mChunkBase[7] = MathlibNEON::CmovRobust( vdupq_n_f32( 0.0f ), r13, degenerateMask );

        mChunkBase[8] = MathlibNEON::CmovRobust( vdupq_n_f32( 0.0f ), r20, degenerateMask );
        mChunkBase[9] = MathlibNEON::CmovRobust( vdupq_n_f32( 0.0f ), r21, degenerateMask );
        mChunkBase[10]= MathlibNEON::CmovRobust( MathlibNEON::ONE, r22, degenerateMask );
        mChunkBase[11]= MathlibNEON::CmovRobust( vdupq_n_f32( 0.0f ), r23, degenerateMask );
    }
    
    inline void ArrayMatrixAf4x3::retain( ArrayMaskR orientation, ArrayMaskR scale )
    {
        ArrayVector3 row0( mChunkBase[0], mChunkBase[1], mChunkBase[2] );
        ArrayVector3 row1( mChunkBase[4], mChunkBase[5], mChunkBase[6] );
        ArrayVector3 row2( mChunkBase[8], mChunkBase[9], mChunkBase[10] );

        ArrayVector3 vScale( row0.length(), row1.length(), row2.length() );
        ArrayVector3 vInvScale( vScale );
        vInvScale.inverseLeaveZeroes();

        row0 *= vInvScale.mChunkBase[0];
        row1 *= vInvScale.mChunkBase[1];
        row2 *= vInvScale.mChunkBase[2];

        vScale.Cmov4( scale, ArrayVector3::UNIT_SCALE );

        row0.Cmov4( orientation, ArrayVector3::UNIT_X );
        row1.Cmov4( orientation, ArrayVector3::UNIT_Y );
        row2.Cmov4( orientation, ArrayVector3::UNIT_Z );

        row0 *= vScale.mChunkBase[0];
        row1 *= vScale.mChunkBase[1];
        row2 *= vScale.mChunkBase[2];

        mChunkBase[0] = row0.mChunkBase[0];
        mChunkBase[1] = row0.mChunkBase[1];
        mChunkBase[2] = row0.mChunkBase[2];

        mChunkBase[4] = row1.mChunkBase[0];
        mChunkBase[5] = row1.mChunkBase[1];
        mChunkBase[6] = row1.mChunkBase[2];

        mChunkBase[8] = row2.mChunkBase[0];
        mChunkBase[9] = row2.mChunkBase[1];
        mChunkBase[10]= row2.mChunkBase[2];
    }
    
    inline void ArrayMatrixAf4x3::streamToAoS( DiMat4 * RESTRICT_ALIAS dst ) const
    {
        //Do not use the unpack version, use the shuffle. Shuffle is faster in k10 processors
        //("The conceptual shuffle" http://developer.amd.com/community/blog/the-conceptual-shuffle/)
        //and the unpack version uses 64-bit moves, which can cause store forwarding issues when
        //then loading them with 128-bit movaps
#define _MM_TRANSPOSE4_SRC_DST_PS(row0, row1, row2, row3, dst0, dst1, dst2, dst3) { \
            float32x4x4_t tmp0, tmp1;               \
            tmp0.val[0] = row0;                     \
            tmp0.val[1] = row1;                     \
            tmp0.val[2] = row2;                     \
            tmp0.val[3] = row3;                     \
            vst4q_f32((float32_t*)&tmp1, tmp0);     \
            dst0 = tmp1.val[0];                     \
            dst1 = tmp1.val[1];                     \
            dst2 = tmp1.val[2];                     \
            dst3 = tmp1.val[3];                     \
        }
        register ArrayFloat m0, m1, m2, m3;

        _MM_TRANSPOSE4_SRC_DST_PS(
                            this->mChunkBase[0], this->mChunkBase[1],
                            this->mChunkBase[2], this->mChunkBase[3],
                            m0, m1, m2, m3 );
        vst1q_f32( dst[0]._m, m0 );
        vst1q_f32( dst[1]._m, m1 );
        vst1q_f32( dst[2]._m, m2 );
        vst1q_f32( dst[3]._m, m3 );
        _MM_TRANSPOSE4_SRC_DST_PS(
                            this->mChunkBase[4], this->mChunkBase[5],
                            this->mChunkBase[6], this->mChunkBase[7],
                            m0, m1, m2, m3 );
        vst1q_f32( dst[0]._m+4, m0 );
        vst1q_f32( dst[1]._m+4, m1 );
        vst1q_f32( dst[2]._m+4, m2 );
        vst1q_f32( dst[3]._m+4, m3 );
        _MM_TRANSPOSE4_SRC_DST_PS(
                            this->mChunkBase[8], this->mChunkBase[9],
                            this->mChunkBase[10], this->mChunkBase[11],
                            m0, m1, m2, m3 );
        vst1q_f32( dst[0]._m+8, m0 );
        vst1q_f32( dst[1]._m+8, m1 );
        vst1q_f32( dst[2]._m+8, m2 );
        vst1q_f32( dst[3]._m+8, m3 );

        vst1q_f32( dst[0]._m+12, MathlibNEON::LAST_AFFINE_COLUMN );
        vst1q_f32( dst[1]._m+12, MathlibNEON::LAST_AFFINE_COLUMN );
        vst1q_f32( dst[2]._m+12, MathlibNEON::LAST_AFFINE_COLUMN );
        vst1q_f32( dst[3]._m+12, MathlibNEON::LAST_AFFINE_COLUMN );
    }
    
    inline void ArrayMatrixAf4x3::storeToAoS( SimpleMatrixAf4x3 * RESTRICT_ALIAS dst ) const
    {
        _MM_TRANSPOSE4_SRC_DST_PS(
                            this->mChunkBase[0], this->mChunkBase[1],
                            this->mChunkBase[2], this->mChunkBase[3],
                            dst[0].mChunkBase[0], dst[1].mChunkBase[0],
                            dst[2].mChunkBase[0], dst[3].mChunkBase[0] );
        _MM_TRANSPOSE4_SRC_DST_PS(
                            this->mChunkBase[4], this->mChunkBase[5],
                            this->mChunkBase[6], this->mChunkBase[7],
                            dst[0].mChunkBase[1], dst[1].mChunkBase[1],
                            dst[2].mChunkBase[1], dst[3].mChunkBase[1] );
        _MM_TRANSPOSE4_SRC_DST_PS(
                            this->mChunkBase[8], this->mChunkBase[9],
                            this->mChunkBase[10], this->mChunkBase[11],
                            dst[0].mChunkBase[2], dst[1].mChunkBase[2],
                            dst[2].mChunkBase[2], dst[3].mChunkBase[2] );
    }
    
    inline void ArrayMatrixAf4x3::streamToAoS( SimpleMatrixAf4x3 * RESTRICT_ALIAS _dst ) const
    {
        register ArrayFloat dst0, dst1, dst2, dst3;
        float *dst = reinterpret_cast<float*>( _dst );

        _MM_TRANSPOSE4_SRC_DST_PS(
                            this->mChunkBase[0], this->mChunkBase[1],
                            this->mChunkBase[2], this->mChunkBase[3],
                            dst0, dst1, dst2, dst3 );

        vst1q_f32( &dst[0],  dst0 );
        vst1q_f32( &dst[12], dst1 );
        vst1q_f32( &dst[24], dst2 );
        vst1q_f32( &dst[36], dst3 );

        _MM_TRANSPOSE4_SRC_DST_PS(
                            this->mChunkBase[4], this->mChunkBase[5],
                            this->mChunkBase[6], this->mChunkBase[7],
                            dst0, dst1, dst2, dst3 );

        vst1q_f32( &dst[4],  dst0 );
        vst1q_f32( &dst[16], dst1 );
        vst1q_f32( &dst[28], dst2 );
        vst1q_f32( &dst[40], dst3 );

        _MM_TRANSPOSE4_SRC_DST_PS(
                            this->mChunkBase[8], this->mChunkBase[9],
                            this->mChunkBase[10], this->mChunkBase[11],
                            dst0, dst1, dst2, dst3 );

        vst1q_f32( &dst[8],  dst0 );
        vst1q_f32( &dst[20], dst1 );
        vst1q_f32( &dst[32], dst2 );
        vst1q_f32( &dst[44], dst3 );
    }
    
    inline void ArrayMatrixAf4x3::loadFromAoS( const DiMat4 * RESTRICT_ALIAS src )
    {
        _MM_TRANSPOSE4_SRC_DST_PS(
                            vld1q_f32( src[0]._m ), vld1q_f32( src[1]._m ), 
                            vld1q_f32( src[2]._m ), vld1q_f32( src[3]._m ),
                            this->mChunkBase[0], this->mChunkBase[1],
                            this->mChunkBase[2], this->mChunkBase[3] );
        _MM_TRANSPOSE4_SRC_DST_PS(
                            vld1q_f32( src[0]._m+4 ), vld1q_f32( src[1]._m+4 ), 
                            vld1q_f32( src[2]._m+4 ), vld1q_f32( src[3]._m+4 ),
                            this->mChunkBase[4], this->mChunkBase[5],
                            this->mChunkBase[6], this->mChunkBase[7] );
        _MM_TRANSPOSE4_SRC_DST_PS(
                            vld1q_f32( src[0]._m+8 ), vld1q_f32( src[1]._m+8 ), 
                            vld1q_f32( src[2]._m+8 ), vld1q_f32( src[3]._m+8 ),
                            this->mChunkBase[8], this->mChunkBase[9],
                            this->mChunkBase[10], this->mChunkBase[11] );
    }
    
    inline void ArrayMatrixAf4x3::loadFromAoS( const SimpleMatrixAf4x3 * RESTRICT_ALIAS src )
    {
        _MM_TRANSPOSE4_SRC_DST_PS(
                            src[0].mChunkBase[0], src[1].mChunkBase[0],
                            src[2].mChunkBase[0], src[3].mChunkBase[0],
                            this->mChunkBase[0], this->mChunkBase[1],
                            this->mChunkBase[2], this->mChunkBase[3] );
        _MM_TRANSPOSE4_SRC_DST_PS(
                            src[0].mChunkBase[1], src[1].mChunkBase[1],
                            src[2].mChunkBase[1], src[3].mChunkBase[1],
                            this->mChunkBase[4], this->mChunkBase[5],
                            this->mChunkBase[6], this->mChunkBase[7] );
        _MM_TRANSPOSE4_SRC_DST_PS(
                            src[0].mChunkBase[2], src[1].mChunkBase[2],
                            src[2].mChunkBase[2], src[3].mChunkBase[2],
                            this->mChunkBase[8], this->mChunkBase[9],
                            this->mChunkBase[10], this->mChunkBase[11] );
    }
    
    inline void ArrayMatrixAf4x3::loadFromAoS( const SimpleMatrixAf4x3 * * RESTRICT_ALIAS src )
    {
        _MM_TRANSPOSE4_SRC_DST_PS(
                            src[0]->mChunkBase[0], src[1]->mChunkBase[0],
                            src[2]->mChunkBase[0], src[3]->mChunkBase[0],
                            this->mChunkBase[0], this->mChunkBase[1],
                            this->mChunkBase[2], this->mChunkBase[3] );
        _MM_TRANSPOSE4_SRC_DST_PS(
                            src[0]->mChunkBase[1], src[1]->mChunkBase[1],
                            src[2]->mChunkBase[1], src[3]->mChunkBase[1],
                            this->mChunkBase[4], this->mChunkBase[5],
                            this->mChunkBase[6], this->mChunkBase[7] );
        _MM_TRANSPOSE4_SRC_DST_PS(
                            src[0]->mChunkBase[2], src[1]->mChunkBase[2],
                            src[2]->mChunkBase[2], src[3]->mChunkBase[2],
                            this->mChunkBase[8], this->mChunkBase[9],
                            this->mChunkBase[10], this->mChunkBase[11] );
    }
    
    #undef _MM_TRANSPOSE4_SRC_DST_PS
}
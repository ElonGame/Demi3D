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
    /** HOW THIS WORKS:

        Instead of writing like 12 times the same code, we use macros:
        DEFINE_OPERATION( ArrayVector3, ArrayVector3, +, vaddq_f32 );
        Means: "define '+' operator that takes both arguments as ArrayVector3 and use
        the vaddq_f32 intrinsic to do the job"

        Note that for scalars (i.e. floats) we use DEFINE_L_SCALAR_OPERATION/DEFINE_R_SCALAR_OPERATION
        depending on whether the scalar is on the left or right side of the operation
        (i.e. 2 * a vs a * 2)
        And for ArrayFloat scalars we use DEFINE_L_OPERATION/DEFINE_R_OPERATION

        As for division, we use specific scalar versions to increase performance (calculate
        the inverse of the scalar once, then multiply) as well as placing asserts in
        case of trying to divide by zero.

        I've considered using templates, but decided against it because wrong operator usage
        would raise cryptic compile error messages, and templates inability to limit which
        types are being used, leave the slight possibility of mixing completely unrelated
        types (i.e. ArrayQuaternion + ArrayVector4) and quietly compiling wrong code.
        Furthermore some platforms may not have decent compilers handling templates for
        major operator usage.

        Advantages:
            * Increased readability
            * Ease of reading & understanding
        Disadvantages:
            * Debugger can't go inside the operator. See workaround.

        As a workaround to the disadvantage, you can compile this code using cl.exe /EP /P /C to
        generate this file with macros replaced as actual code (very handy!)
    */
    // Arithmetic operations
#define DEFINE_OPERATION( leftClass, rightClass, op, op_func )\
    inline ArrayVector3 operator op ( const leftClass &lhs, const rightClass &rhs )\
    {\
        const ArrayFloat * RESTRICT_ALIAS lhsChunkBase = lhs.mChunkBase;\
        const ArrayFloat * RESTRICT_ALIAS rhsChunkBase = rhs.mChunkBase;\
        return ArrayVector3(\
                op_func( lhsChunkBase[0], rhsChunkBase[0] ),\
                op_func( lhsChunkBase[1], rhsChunkBase[1] ),\
                op_func( lhsChunkBase[2], rhsChunkBase[2] ) );\
       }
#define DEFINE_L_SCALAR_OPERATION( leftType, rightClass, op, op_func )\
    inline ArrayVector3 operator op ( const leftType fScalar, const rightClass &rhs )\
    {\
        ArrayFloat lhs = vdupq_n_f32( fScalar );\
        return ArrayVector3(\
                op_func( lhs, rhs.mChunkBase[0] ),\
                op_func( lhs, rhs.mChunkBase[1] ),\
                op_func( lhs, rhs.mChunkBase[2] ) );\
    }
#define DEFINE_R_SCALAR_OPERATION( leftClass, rightType, op, op_func )\
    inline ArrayVector3 operator op ( const leftClass &lhs, const rightType fScalar )\
    {\
        ArrayFloat rhs = vdupq_n_f32( fScalar );\
        return ArrayVector3(\
                op_func( lhs.mChunkBase[0], rhs ),\
                op_func( lhs.mChunkBase[1], rhs ),\
                op_func( lhs.mChunkBase[2], rhs ) );\
    }
#define DEFINE_L_OPERATION( leftType, rightClass, op, op_func )\
    inline ArrayVector3 operator op ( const leftType lhs, const rightClass &rhs )\
    {\
        return ArrayVector3(\
                op_func( lhs, rhs.mChunkBase[0] ),\
                op_func( lhs, rhs.mChunkBase[1] ),\
                op_func( lhs, rhs.mChunkBase[2] ) );\
    }
#define DEFINE_R_OPERATION( leftClass, rightType, op, op_func )\
    inline ArrayVector3 operator op ( const leftClass &lhs, const rightType rhs )\
    {\
        return ArrayVector3(\
                op_func( lhs.mChunkBase[0], rhs ),\
                op_func( lhs.mChunkBase[1], rhs ),\
                op_func( lhs.mChunkBase[2], rhs ) );\
    }

#define DEFINE_L_SCALAR_DIVISION( leftType, rightClass, op, op_func )\
    inline ArrayVector3 operator op ( const leftType fScalar, const rightClass &rhs )\
    {\
        ArrayFloat lhs = vdupq_n_f32( fScalar );\
        return ArrayVector3(\
                op_func( lhs, rhs.mChunkBase[0] ),\
                op_func( lhs, rhs.mChunkBase[1] ),\
                op_func( lhs, rhs.mChunkBase[2] ) );\
    }
#define DEFINE_R_SCALAR_DIVISION( leftClass, rightType, op, op_func )\
    inline ArrayVector3 operator op ( const leftClass &lhs, const rightType fScalar )\
    {\
        assert( fScalar != 0.0 );\
        float fInv = 1.0f / fScalar;\
        ArrayFloat rhs = vdupq_n_f32( fInv );\
        return ArrayVector3(\
                op_func( lhs.mChunkBase[0], rhs ),\
                op_func( lhs.mChunkBase[1], rhs ),\
                op_func( lhs.mChunkBase[2], rhs ) );\
    }

#ifdef NDEBUG
    #define ASSERT_DIV_BY_ZERO( values ) ((void)0)
#else
    #define ASSERT_DIV_BY_ZERO( values ) {\
                DI_ASSERT( vmovemaskq_u32( vceqq_f32( values, vdupq_n_f32(0.0f) ) ) == 0 &&\
                "One of the 4 floats is a zero. Can't divide by zero" ); }
#endif

#define DEFINE_L_DIVISION( leftType, rightClass, op, op_func )\
    inline ArrayVector3 operator op ( const leftType lhs, const rightClass &rhs )\
    {\
        return ArrayVector3(\
                op_func( lhs, rhs.mChunkBase[0] ),\
                op_func( lhs, rhs.mChunkBase[1] ),\
                op_func( lhs, rhs.mChunkBase[2] ) );\
    }
#define DEFINE_R_DIVISION( leftClass, rightType, op, op_func )\
    inline ArrayVector3 operator op ( const leftClass &lhs, const rightType r )\
    {\
        ASSERT_DIV_BY_ZERO( r );\
        ArrayFloat rhs = MathlibNEON::Inv4( r );\
        return ArrayVector3(\
                op_func( lhs.mChunkBase[0], rhs ),\
                op_func( lhs.mChunkBase[1], rhs ),\
                op_func( lhs.mChunkBase[2], rhs ) );\
    }

    // Update operations
#define DEFINE_UPDATE_OPERATION( leftClass, op, op_func )\
    inline void ArrayVector3::operator op ( const leftClass &a )\
    {\
        ArrayFloat * RESTRICT_ALIAS chunkBase = mChunkBase;\
        const ArrayFloat * RESTRICT_ALIAS aChunkBase = a.mChunkBase;\
        chunkBase[0] = op_func( chunkBase[0], aChunkBase[0] );\
        chunkBase[1] = op_func( chunkBase[1], aChunkBase[1] );\
        chunkBase[2] = op_func( chunkBase[2], aChunkBase[2] );\
    }
#define DEFINE_UPDATE_R_SCALAR_OPERATION( rightType, op, op_func )\
    inline void ArrayVector3::operator op ( const rightType fScalar )\
    {\
        ArrayFloat a = vdupq_n_f32( fScalar );\
        mChunkBase[0] = op_func( mChunkBase[0], a );\
        mChunkBase[1] = op_func( mChunkBase[1], a );\
        mChunkBase[2] = op_func( mChunkBase[2], a );\
    }
#define DEFINE_UPDATE_R_OPERATION( rightType, op, op_func )\
    inline void ArrayVector3::operator op ( const rightType a )\
    {\
        mChunkBase[0] = op_func( mChunkBase[0], a );\
        mChunkBase[1] = op_func( mChunkBase[1], a );\
        mChunkBase[2] = op_func( mChunkBase[2], a );\
    }
#define DEFINE_UPDATE_DIVISION( leftClass, op, op_func )\
    inline void ArrayVector3::operator op ( const leftClass &a )\
    {\
        ArrayFloat * RESTRICT_ALIAS chunkBase = mChunkBase;\
        const ArrayFloat * RESTRICT_ALIAS aChunkBase = a.mChunkBase;\
        chunkBase[0] = op_func( chunkBase[0], aChunkBase[0] );\
        chunkBase[1] = op_func( chunkBase[1], aChunkBase[1] );\
        chunkBase[2] = op_func( chunkBase[2], aChunkBase[2] );\
    }
#define DEFINE_UPDATE_R_SCALAR_DIVISION( rightType, op, op_func )\
    inline void ArrayVector3::operator op ( const rightType fScalar )\
    {\
        assert( fScalar != 0.0 );\
        float fInv = 1.0f / fScalar;\
        ArrayFloat a = vdupq_n_f32( fInv );\
        mChunkBase[0] = op_func( mChunkBase[0], a );\
        mChunkBase[1] = op_func( mChunkBase[1], a );\
        mChunkBase[2] = op_func( mChunkBase[2], a );\
    }
#define DEFINE_UPDATE_R_DIVISION( rightType, op, op_func )\
    inline void ArrayVector3::operator op ( const rightType _a )\
    {\
        ASSERT_DIV_BY_ZERO( _a );\
        ArrayFloat a = MathlibNEON::Inv4( _a );\
        mChunkBase[0] = op_func( mChunkBase[0], a );\
        mChunkBase[1] = op_func( mChunkBase[1], a );\
        mChunkBase[2] = op_func( mChunkBase[2], a );\
    }

    inline const ArrayVector3& ArrayVector3::operator + () const
    {
        return *this;
    };
    
    inline ArrayVector3 ArrayVector3::operator - () const
    {
        return ArrayVector3(
            veorq_s32( mChunkBase[0], MathlibNEON::SIGN_MASK ), //-x
            veorq_s32( mChunkBase[1], MathlibNEON::SIGN_MASK ), //-y
            veorq_s32( mChunkBase[2], MathlibNEON::SIGN_MASK ) );   //-z
    }
    

    // + Addition
    DEFINE_OPERATION( ArrayVector3, ArrayVector3, +, vaddq_f32 );
    DEFINE_L_SCALAR_OPERATION( float, ArrayVector3, +, vaddq_f32 );
    DEFINE_R_SCALAR_OPERATION( ArrayVector3, float, +, vaddq_f32 );

    DEFINE_L_OPERATION( ArrayFloat, ArrayVector3, +, vaddq_f32 );
    DEFINE_R_OPERATION( ArrayVector3, ArrayFloat, +, vaddq_f32 );

    // - Subtraction
    DEFINE_OPERATION( ArrayVector3, ArrayVector3, -, vsubq_f32 );
    DEFINE_L_SCALAR_OPERATION( float, ArrayVector3, -, vsubq_f32 );
    DEFINE_R_SCALAR_OPERATION( ArrayVector3, float, -, vsubq_f32 );

    DEFINE_L_OPERATION( ArrayFloat, ArrayVector3, -, vsubq_f32 );
    DEFINE_R_OPERATION( ArrayVector3, ArrayFloat, -, vsubq_f32 );

    // * Multiplication
    DEFINE_OPERATION( ArrayVector3, ArrayVector3, *, vmulq_f32 );
    DEFINE_L_SCALAR_OPERATION( float, ArrayVector3, *, vmulq_f32 );
    DEFINE_R_SCALAR_OPERATION( ArrayVector3, float, *, vmulq_f32 );

    DEFINE_L_OPERATION( ArrayFloat, ArrayVector3, *, vmulq_f32 );
    DEFINE_R_OPERATION( ArrayVector3, ArrayFloat, *, vmulq_f32 );

    // / Division (scalar versions use mul instead of div, because they mul against the reciprocal)
    DEFINE_OPERATION( ArrayVector3, ArrayVector3, /, vdivq_f32 );
    DEFINE_L_SCALAR_DIVISION( float, ArrayVector3, /, vdivq_f32 );
    DEFINE_R_SCALAR_DIVISION( ArrayVector3, float, /, vmulq_f32 );

    DEFINE_L_DIVISION( ArrayFloat, ArrayVector3, /, vdivq_f32 );
    DEFINE_R_DIVISION( ArrayVector3, ArrayFloat, /, vmulq_f32 );

    inline ArrayVector3 ArrayVector3::Cmov4( const ArrayVector3 &arg1, const ArrayVector3 &arg2, ArrayFloat mask )
    {
        return ArrayVector3(
                MathlibNEON::Cmov4( arg1.mChunkBase[0], arg2.mChunkBase[0], mask ),
                MathlibNEON::Cmov4( arg1.mChunkBase[1], arg2.mChunkBase[1], mask ),
                MathlibNEON::Cmov4( arg1.mChunkBase[2], arg2.mChunkBase[2], mask ) );
    }

    // Update operations
    // +=
    DEFINE_UPDATE_OPERATION(            ArrayVector3,       +=, vaddq_f32 );
    DEFINE_UPDATE_R_SCALAR_OPERATION(   float,               +=, vaddq_f32 );
    DEFINE_UPDATE_R_OPERATION(          ArrayFloat,          +=, vaddq_f32 );

    // -=
    DEFINE_UPDATE_OPERATION(            ArrayVector3,       -=, vsubq_f32 );
    DEFINE_UPDATE_R_SCALAR_OPERATION(   float,               -=, vsubq_f32 );
    DEFINE_UPDATE_R_OPERATION(          ArrayFloat,          -=, vsubq_f32 );

    // *=
    DEFINE_UPDATE_OPERATION(            ArrayVector3,       *=, vmulq_f32 );
    DEFINE_UPDATE_R_SCALAR_OPERATION(   float,               *=, vmulq_f32 );
    DEFINE_UPDATE_R_OPERATION(          ArrayFloat,          *=, vmulq_f32 );

    // /=
    DEFINE_UPDATE_DIVISION(             ArrayVector3,       /=, vdivq_f32 );
    DEFINE_UPDATE_R_SCALAR_DIVISION(    float,               /=, vmulq_f32 );
    DEFINE_UPDATE_R_DIVISION(           ArrayFloat,          /=, vmulq_f32 );

    //Functions
    
    inline ArrayFloat ArrayVector3::length() const
    {
        return
        vrsqrteq_f32( vaddq_f32( vaddq_f32(                 //sqrt(
                vmulq_f32( mChunkBase[0], mChunkBase[0] ),  //(x * x +
                vmulq_f32( mChunkBase[1], mChunkBase[1] ) ),    //y * y) +
            vmulq_f32( mChunkBase[2], mChunkBase[2] ) ) );  //z * z )
    }
    
    inline ArrayFloat ArrayVector3::squaredLength() const
    {
        return
        vaddq_f32( vaddq_f32(
            vmulq_f32( mChunkBase[0], mChunkBase[0] ),  //(x * x +
            vmulq_f32( mChunkBase[1], mChunkBase[1] ) ),    //y * y) +
        vmulq_f32( mChunkBase[2], mChunkBase[2] ) );        //z * z )
    }
    
    inline ArrayFloat ArrayVector3::distance( const ArrayVector3& rhs ) const
    {
        return (*this - rhs).length();
    }
    
    inline ArrayFloat ArrayVector3::squaredDistance( const ArrayVector3& rhs ) const
    {
        return (*this - rhs).squaredLength();
    }
    
    inline ArrayFloat ArrayVector3::dotProduct( const ArrayVector3& vec ) const
    {
        return
        vaddq_f32( vaddq_f32(
            vmulq_f32( mChunkBase[0], vec.mChunkBase[0] ) , //( x * vec.x   +
            vmulq_f32( mChunkBase[1], vec.mChunkBase[1] ) ),    //  y * vec.y ) +
            vmulq_f32( mChunkBase[2], vec.mChunkBase[2] ) );    //  z * vec.z
    }
    
    inline ArrayFloat ArrayVector3::absDotProduct( const ArrayVector3& vec ) const
    {
        return
        vaddq_f32( vaddq_f32(
            MathlibNEON::Abs4( vmulq_f32( mChunkBase[0], vec.mChunkBase[0] ) ), //( abs( x * vec.x )   +
            MathlibNEON::Abs4( vmulq_f32( mChunkBase[1], vec.mChunkBase[1] ) ) ),//  abs( y * vec.y ) ) +
            MathlibNEON::Abs4( vmulq_f32( mChunkBase[2], vec.mChunkBase[2] ) ) );//  abs( z * vec.z )
    }
    
    inline void ArrayVector3::normalise( void )
    {
        ArrayFloat sqLength = vaddq_f32( vaddq_f32(
            vmulq_f32( mChunkBase[0], mChunkBase[0] ),  //(x * x +
            vmulq_f32( mChunkBase[1], mChunkBase[1] ) ),    //y * y) +
        vmulq_f32( mChunkBase[2], mChunkBase[2] ) );        //z * z )

        //Convert sqLength's 0s into 1, so that zero vectors remain as zero
        //Denormals are treated as 0 during the check.
        //Note: We could create a mask now and nuke nans after InvSqrt, however
        //generating the nans could impact performance in some architectures
        sqLength = MathlibNEON::Cmov4( sqLength, MathlibNEON::ONE,
                                        vcgtq_f32( sqLength, MathlibNEON::FLOAT_MIN ) );
        ArrayFloat invLength = MathlibNEON::InvSqrtNonZero4( sqLength );
        mChunkBase[0] = vmulq_f32( mChunkBase[0], invLength ); //x * invLength
        mChunkBase[1] = vmulq_f32( mChunkBase[1], invLength ); //y * invLength
        mChunkBase[2] = vmulq_f32( mChunkBase[2], invLength ); //z * invLength
    }
    
    inline ArrayVector3 ArrayVector3::crossProduct( const ArrayVector3& rkVec ) const
    {
        return ArrayVector3(
            vsubq_f32(
                vmulq_f32( mChunkBase[1], rkVec.mChunkBase[2] ),
                vmulq_f32( mChunkBase[2], rkVec.mChunkBase[1] ) ),  //y * rkVec.z - z * rkVec.y
            vsubq_f32(
                vmulq_f32( mChunkBase[2], rkVec.mChunkBase[0] ),
                vmulq_f32( mChunkBase[0], rkVec.mChunkBase[2] ) ),  //z * rkVec.x - x * rkVec.z
            vsubq_f32(
                vmulq_f32( mChunkBase[0], rkVec.mChunkBase[1] ),
                vmulq_f32( mChunkBase[1], rkVec.mChunkBase[0] ) ) );    //x * rkVec.y - y * rkVec.x
    }
    
    inline ArrayVector3 ArrayVector3::midPoint( const ArrayVector3& rkVec ) const
    {
        return ArrayVector3(
            vmulq_f32( vaddq_f32( mChunkBase[0], rkVec.mChunkBase[0] ), MathlibNEON::HALF ),
            vmulq_f32( vaddq_f32( mChunkBase[1], rkVec.mChunkBase[1] ), MathlibNEON::HALF ),
            vmulq_f32( vaddq_f32( mChunkBase[2], rkVec.mChunkBase[2] ), MathlibNEON::HALF ) );
    }
    
    inline void ArrayVector3::makeFloor( const ArrayVector3& cmp )
    {
        ArrayFloat * RESTRICT_ALIAS aChunkBase = mChunkBase;
        const ArrayFloat * RESTRICT_ALIAS bChunkBase = cmp.mChunkBase;
        aChunkBase[0] = vminq_f32( aChunkBase[0], bChunkBase[0] );
        aChunkBase[1] = vminq_f32( aChunkBase[1], bChunkBase[1] );
        aChunkBase[2] = vminq_f32( aChunkBase[2], bChunkBase[2] );
    }
    
    inline void ArrayVector3::makeCeil( const ArrayVector3& cmp )
    {
        ArrayFloat * RESTRICT_ALIAS aChunkBase = mChunkBase;
        const ArrayFloat * RESTRICT_ALIAS bChunkBase = cmp.mChunkBase;
        aChunkBase[0] = vmaxq_f32( aChunkBase[0], bChunkBase[0] );
        aChunkBase[1] = vmaxq_f32( aChunkBase[1], bChunkBase[1] );
        aChunkBase[2] = vmaxq_f32( aChunkBase[2], bChunkBase[2] );
    }
    
    inline ArrayFloat ArrayVector3::getMinComponent() const
    {
        return vminq_f32( mChunkBase[0], vminq_f32( mChunkBase[1], mChunkBase[2] ) );
    }
    
    inline ArrayFloat ArrayVector3::getMaxComponent() const
    {
        return vmaxq_f32( mChunkBase[0], vmaxq_f32( mChunkBase[1], mChunkBase[2] ) );
    }
    
    inline void ArrayVector3::setToSign()
    {
        // x = 1.0f | (x & 0x80000000)
        ArrayFloat signMask = vdupq_n_f32( -0.0f );
        mChunkBase[0] = vorrq_f32( MathlibNEON::ONE, vandq_s32( signMask, mChunkBase[0] ) );
        mChunkBase[1] = vorrq_f32( MathlibNEON::ONE, vandq_s32( signMask, mChunkBase[1] ) );
        mChunkBase[2] = vorrq_f32( MathlibNEON::ONE, vandq_s32( signMask, mChunkBase[2] ) );
    }
    
    inline ArrayVector3 ArrayVector3::perpendicular( void ) const
    {
        ArrayVector3 perp = this->crossProduct( ArrayVector3::UNIT_X );

        const uint32x4_t mask = vcleq_f32( perp.squaredLength(), MathlibNEON::fSqEpsilon );
        // Check length
        if( vmovemaskq_u32( mask ) )
        {
            /* One or more of these vectors are the X axis multiplied by a scalar,
               so we have to use another axis for those.
            */
            ArrayVector3 perp1 = this->crossProduct( ArrayVector3::UNIT_Y );
            perp.mChunkBase[0] = MathlibNEON::Cmov4( perp1.mChunkBase[0], perp.mChunkBase[0], mask );
            perp.mChunkBase[1] = MathlibNEON::Cmov4( perp1.mChunkBase[1], perp.mChunkBase[1], mask );
            perp.mChunkBase[2] = MathlibNEON::Cmov4( perp1.mChunkBase[2], perp.mChunkBase[2], mask );
        }
        perp.normalise();

        return perp;
    }
    
    inline ArrayVector3 ArrayVector3::normalisedCopy( void ) const
    {
        ArrayFloat sqLength = vaddq_f32( vaddq_f32(
            vmulq_f32( mChunkBase[0], mChunkBase[0] ),  //(x * x +
            vmulq_f32( mChunkBase[1], mChunkBase[1] ) ),    //y * y) +
        vmulq_f32( mChunkBase[2], mChunkBase[2] ) );        //z * z )

        //Convert sqLength's 0s into 1, so that zero vectors remain as zero
        //Denormals are treated as 0 during the check.
        //Note: We could create a mask now and nuke nans after InvSqrt, however
        //generating the nans could impact performance in some architectures
        sqLength = MathlibNEON::Cmov4( sqLength, MathlibNEON::ONE,
                                        vcgtq_f32( sqLength, MathlibNEON::FLOAT_MIN ) );
        ArrayFloat invLength = MathlibNEON::InvSqrtNonZero4( sqLength );

        return ArrayVector3(
            vmulq_f32( mChunkBase[0], invLength ),  //x * invLength
            vmulq_f32( mChunkBase[1], invLength ),  //y * invLength
            vmulq_f32( mChunkBase[2], invLength ) );    //z * invLength
    }
    
    inline ArrayVector3 ArrayVector3::reflect( const ArrayVector3& normal ) const
    {
        const ArrayFloat twoPointZero = vdupq_n_f32( 2.0f );
        return ( *this - ( vmulq_f32( twoPointZero, this->dotProduct( normal ) ) * normal ) );
    }
    
    inline void ArrayVector3::inverseLeaveZeroes( void )
    {
        //Use InvNonZero, we're gonna nuke the NaNs anyway.
        mChunkBase[0] = MathlibNEON::CmovRobust( mChunkBase[0], MathlibNEON::InvNonZero4(mChunkBase[0]),
        vceqq_f32( mChunkBase[0], vdupq_n_f32(0.0f) ) );
        mChunkBase[1] = MathlibNEON::CmovRobust( mChunkBase[1], MathlibNEON::InvNonZero4(mChunkBase[1]),
        vceqq_f32( mChunkBase[1], vdupq_n_f32(0.0f) ) );
        mChunkBase[2] = MathlibNEON::CmovRobust( mChunkBase[2], MathlibNEON::InvNonZero4(mChunkBase[2]),
        vceqq_f32( mChunkBase[2], vdupq_n_f32(0.0f) ) );
    }
    
    inline int ArrayVector3::isNaN( void ) const
    {
        ArrayInt mask = vandq_s32( vandq_s32(
            vceqq_f32( mChunkBase[0], mChunkBase[0] ),
            vceqq_f32( mChunkBase[1], mChunkBase[1] ) ),
            vceqq_f32( mChunkBase[2], mChunkBase[2] ) );

        return vmovemaskq_u32( mask ) ^ 0x0000000f;
    }
    
    inline ArrayVector3 ArrayVector3::primaryAxis( void ) const
    {
        // We could've used some operators, i.e.
        // xVec = MathlibNEON::Cmov( ArrayVector3::UNIT_X, ArrayVector3::NEGATIVE_UNIT_X )
        // and so forth, which would've increased readability considerably. However,
        // MSVC's compiler ability to do constant propagation & remove dead code sucks,
        // which means it would try to cmov the Y & Z component even though we already
        // know it's always zero for both +x & -x. Therefore, we do it the manual
        // way. Doing this the "human readable way" results in massive amounts of wasted
        // instructions and stack memory abuse.
        // See DiVec3::primaryAxis() to understand what's actually going on.
        ArrayFloat absx = MathlibNEON::Abs4( mChunkBase[0] );
        ArrayFloat absy = MathlibNEON::Abs4( mChunkBase[1] );
        ArrayFloat absz = MathlibNEON::Abs4( mChunkBase[2] );

        //xVec = x > 0 ? DiVec3::UNIT_X : DiVec3::NEGATIVE_UNIT_X;
        ArrayFloat sign = MathlibNEON::Cmov4( vdupq_n_f32( 1.0f ), vdupq_n_f32( -1.0f ),
                                            vcgtq_f32( mChunkBase[0], vdupq_n_f32(0.0f) ) );
        ArrayVector3 xVec( sign, vdupq_n_f32(0.0f), vdupq_n_f32(0.0f) );

        //yVec = y > 0 ? DiVec3::UNIT_Y : DiVec3::NEGATIVE_UNIT_Y;
        sign = MathlibNEON::Cmov4( vdupq_n_f32( 1.0f ), vdupq_n_f32( -1.0f ),
                                    vcgtq_f32( mChunkBase[1], vdupq_n_f32(0.0f) ) );
        ArrayVector3 yVec( vdupq_n_f32(0.0f), sign, vdupq_n_f32(0.0f) );

        //zVec = z > 0 ? DiVec3::UNIT_Z : DiVec3::NEGATIVE_UNIT_Z;
        sign = MathlibNEON::Cmov4( vdupq_n_f32( 1.0f ), vdupq_n_f32( -1.0f ),
                                    vcgtq_f32( mChunkBase[2], vdupq_n_f32(0.0f) ) );
        ArrayVector3 zVec( vdupq_n_f32(0.0f), vdupq_n_f32(0.0f), sign );

        //xVec = absx > absz ? xVec : zVec
        ArrayFloat mask = vcgtq_f32( absx, absz );
        xVec.mChunkBase[0] = MathlibNEON::Cmov4( xVec.mChunkBase[0], zVec.mChunkBase[0], mask );
        xVec.mChunkBase[2] = MathlibNEON::Cmov4( xVec.mChunkBase[2], zVec.mChunkBase[2], mask );

        //yVec = absy > absz ? yVec : zVec
        mask = vcgtq_f32( absy, absz );
        yVec.mChunkBase[1] = MathlibNEON::Cmov4( yVec.mChunkBase[1], zVec.mChunkBase[1], mask );
        yVec.mChunkBase[2] = MathlibNEON::Cmov4( yVec.mChunkBase[2], zVec.mChunkBase[2], mask );

        yVec.Cmov4( vcgtq_f32( absx, absy ), xVec );
        return yVec;
    }
    
    inline DiVec3 ArrayVector3::collapseMin( void ) const
    {
        DEMI_ALIGNED_DECL( float, vals[4], DEMI_SIMD_ALIGNMENT );
//      ArrayFloat aosVec0, aosVec1, aosVec2, aosVec3;
        float min0 = MathlibNEON::CollapseMin(mChunkBase[0]);
        float min1 = MathlibNEON::CollapseMin(mChunkBase[1]);
        float min2 = MathlibNEON::CollapseMin(mChunkBase[2]);

        ArrayFloat minArray = { min0, min1, min2, std::numeric_limits<float>::infinity() };
        float min = MathlibNEON::CollapseMin(minArray);
//        min = vminq_f32(mChunkBase[0], mChunkBase[1]);
//        min = vminq_f32(min, mChunkBase[2]);

//        ArrayFloat a_lo, a_hi, min;
//        a_lo = vget_low_f32(a);
//        a_hi = vget_high_f32(a);
//        min = vpmin_f32(a_lo, a_hi);
//        min = vpmin_f32(min, min);
//
//        return vget_lane_f32(min, 0);

        //Transpose XXXX YYYY ZZZZ to XYZZ XYZZ XYZZ XYZZ
//      ArrayFloat tmp2, tmp0;
//      tmp0   = vshuf_f32( mChunkBase[0], mChunkBase[1], 0x44 );
//        tmp2   = vshuf_f32( mChunkBase[0], mChunkBase[1], 0xEE );
//
//        aosVec0 = vshuf_f32( tmp0, mChunkBase[2], 0x08 );
//      aosVec1 = vshuf_f32( tmp0, mChunkBase[2], 0x5D );
//      aosVec2 = vshuf_f32( tmp2, mChunkBase[2], 0xA8 );
//      aosVec3 = vshuf_f32( tmp2, mChunkBase[2], 0xFD );
//
//      //Do the actual operation
//      aosVec0 = vminq_f32( aosVec0, aosVec1 );
//      aosVec2 = vminq_f32( aosVec2, aosVec3 );
//      aosVec0 = vminq_f32( aosVec0, aosVec2 );

        vst1q_f32( vals, vdupq_n_f32(min) );

        return DiVec3( vals[0], vals[1], vals[2] );
    }
    
    inline DiVec3 ArrayVector3::collapseMax( void ) const
    {
        DEMI_ALIGNED_DECL( float, vals[4], DEMI_SIMD_ALIGNMENT );
//      ArrayFloat aosVec0, aosVec1, aosVec2, aosVec3;
        float max0 = MathlibNEON::CollapseMax(mChunkBase[0]);
        float max1 = MathlibNEON::CollapseMax(mChunkBase[1]);
        float max2 = MathlibNEON::CollapseMax(mChunkBase[2]);

        ArrayFloat maxArray = { max0, max1, max2, -std::numeric_limits<float>::infinity() };
        float max = MathlibNEON::CollapseMax(maxArray);
//        ArrayFloat max;
//        max = vmaxq_f32(mChunkBase[0], mChunkBase[1]);
//        max = vmaxq_f32(max, mChunkBase[2]);

        //Transpose XXXX YYYY ZZZZ to XYZZ XYZZ XYZZ XYZZ
//      ArrayFloat tmp2, tmp0;
//      tmp0   = vshuf_f32( mChunkBase[0], mChunkBase[1], 0x44 );
//        tmp2   = vshuf_f32( mChunkBase[0], mChunkBase[1], 0xEE );
//
//        aosVec0 = vshuf_f32( tmp0, mChunkBase[2], 0x08 );
//      aosVec1 = vshuf_f32( tmp0, mChunkBase[2], 0x5D );
//      aosVec2 = vshuf_f32( tmp2, mChunkBase[2], 0xA8 );
//      aosVec3 = vshuf_f32( tmp2, mChunkBase[2], 0xFD );
//
//      //Do the actual operation
//      aosVec0 = vmaxq_f32( aosVec0, aosVec1 );
//      aosVec2 = vmaxq_f32( aosVec2, aosVec3 );
//      aosVec0 = vmaxq_f32( aosVec0, aosVec2 );

        vst1q_f32( vals, vdupq_n_f32(max) );

        return DiVec3( vals[0], vals[1], vals[2] );
    }
    
    inline void ArrayVector3::Cmov4( ArrayFloat mask, const ArrayVector3 &replacement )
    {
        ArrayFloat * RESTRICT_ALIAS aChunkBase = mChunkBase;
        const ArrayFloat * RESTRICT_ALIAS bChunkBase = replacement.mChunkBase;
        aChunkBase[0] = MathlibNEON::Cmov4( aChunkBase[0], bChunkBase[0], mask );
        aChunkBase[1] = MathlibNEON::Cmov4( aChunkBase[1], bChunkBase[1], mask );
        aChunkBase[2] = MathlibNEON::Cmov4( aChunkBase[2], bChunkBase[2], mask );
    }
    
    inline void ArrayVector3::CmovRobust( ArrayFloat mask, const ArrayVector3 &replacement )
    {
        ArrayFloat * RESTRICT_ALIAS aChunkBase = mChunkBase;
        const ArrayFloat * RESTRICT_ALIAS bChunkBase = replacement.mChunkBase;
        aChunkBase[0] = MathlibNEON::CmovRobust( aChunkBase[0], bChunkBase[0], mask );
        aChunkBase[1] = MathlibNEON::CmovRobust( aChunkBase[1], bChunkBase[1], mask );
        aChunkBase[2] = MathlibNEON::CmovRobust( aChunkBase[2], bChunkBase[2], mask );
    }
    

#undef DEFINE_OPERATION
#undef DEFINE_L_SCALAR_OPERATION
#undef DEFINE_R_SCALAR_OPERATION
#undef DEFINE_L_OPERATION
#undef DEFINE_R_OPERATION
#undef DEFINE_DIVISION
#undef DEFINE_L_DIVISION
#undef DEFINE_R_SCALAR_DIVISION
#undef DEFINE_L_SCALAR_DIVISION
#undef DEFINE_R_DIVISION

#undef DEFINE_UPDATE_OPERATION
#undef DEFINE_UPDATE_R_SCALAR_OPERATION
#undef DEFINE_UPDATE_R_OPERATION
#undef DEFINE_UPDATE_DIVISION
#undef DEFINE_UPDATE_R_SCALAR_DIVISION
#undef DEFINE_UPDATE_R_DIVISION
}

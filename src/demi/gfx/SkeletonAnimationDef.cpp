
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

#include "SkeletonAnimationDef.h"

#include "Math/Array/Mathlib.h"
#include "Math/Array/Transform.h"
#include "KfTransformArrayMemoryManager.h"

#include "Skeleton.h"
#include "Bone.h"
#include "Animation.h"
#include "AnimationClip.h"
#include "KeyFrame.h"

namespace Demi
{
    SkeletonAnimationDef::SkeletonAnimationDef() :
        mNumFrames( 0 ),
        mOriginalFrameRate( 25.0f ),
        mKfTransformMemoryManager( 0 )
    {
    }
    
    SkeletonAnimationDef::~SkeletonAnimationDef()
    {
        mTracks.clear();

        if( mKfTransformMemoryManager )
        {
            mKfTransformMemoryManager->destroy();
            delete mKfTransformMemoryManager;
            mKfTransformMemoryManager = 0;
        }
    }
    
    inline uint32 SkeletonAnimationDef::slotToBlockIdx( uint32 slotIdx ) const
    {
        return (slotIdx & 0xFF000000) | ((slotIdx & 0x00FFFFFF) / ARRAY_PACKED_REALS);
    }
    
    inline uint32 SkeletonAnimationDef::blockIdxToSlotStart( uint32 blockIdx ) const
    {
        return (blockIdx & 0xFF000000) | ((blockIdx & 0x00FFFFFF) * ARRAY_PACKED_REALS);
    }
    
    void SkeletonAnimationDef::build( DiSkeleton *skeleton, DiAnimation *animation, float frameRate )
    {
        mOriginalFrameRate = frameRate;
        mNumFrames = animation->GetLength() * frameRate;

        //Terminology:
        // Bone Index:
        //  It's the bone index given by skeleton. i.e. getBone( boneIdx )
        // Slot Index
        //  It's the place in SoA slots the bone will end up with, with an index containing
        //  the parent level in the high 8 bits.
        //          For example the first root bone is 0
        //          The first bone child of root is (1 << 24) | (0 & 0x00FFFFF)
        //          The second bone child of root is (1 << 24) | (1 & 0x00FFFFF)
        //          The first bone child of child of root is (2 << 24) | (0 & 0x00FFFFF)
        //
        // Block Index.
        //  It's the same as slot index, but the slot part (low 24 bits) is divided by ARRAY_PACKED_REALS
        //  For example, ARRAY_PACKED_REALS = 4, slots 0 1 2 & 3 are in block 0

        //Converts Bone Index -> Slot Index
        DiVector<uint32> boneToSlot;
        boneToSlot.reserve( skeleton->GetNumBones() );

        //Converts Slot Index -> Bone Index
        DiMap<uint32, uint32> slotToBone;

        for (size_t i = 0; i<skeleton->GetNumBones(); ++i)
        {
            DiBone *bone = skeleton->GetBone( i );

            size_t depthLevel = 0;
            DiNode* parentBone = bone;
            while( (parentBone = parentBone->GetParent()) )
                ++depthLevel;

            size_t offset = 0;
            auto itor = boneToSlot.begin();
            auto end = boneToSlot.end();
            while( itor != end )
            {
                if( (*itor >> 24) == depthLevel )
                    ++offset;
                ++itor;
            }

            //Build the map that lets us know the final slot bone index that will be
            //assigned to this bone (to get the block we still need to divide by ARRAY_PACKED_REALS)
            boneToSlot.push_back( static_cast<uint32>((depthLevel << 24) | (offset & 0x00FFFFFF)) );
            slotToBone[boneToSlot.back()] = static_cast<uint32>(i);
        }

        //1st Pass: Count the number of keyframes, so we know how
        //much memory to allocate, as we don't listen for resizes.
        //We also build a list of unique keyframe timestamps per block
        //(i.e. merge the keyframes from two bones that the same block)
        auto itor = animation->GetNodeClipsIterator();
        {
            //Count the number of blocks needed by counting the number of unique keyframes per block.
            //i.e. When ARRAY_PACKED_REALS = 4; if 2 bones are in the same block and have the same
            //keyframe time value, we'll need 4 slots (aka. 1 block). If those 2 bones have two
            //different time values, we'll need 8 slots.
            TimestampVec emptyVec;
            TimestampsPerBlock timestampsByBlock;

            while( itor.HasMoreElements() )
            {
                size_t boneIdx = itor.PeekNextKey();
                DiNodeClip* track = itor.GetNext();

                if (track->GetNumFrames() > 0)
                {
                    uint32 slotIdx = boneToSlot[boneIdx];
                    uint32 blockIdx = slotToBlockIdx( slotIdx );

                    TimestampsPerBlock::iterator itKeyframes = timestampsByBlock.find( blockIdx );
                    if( itKeyframes == timestampsByBlock.end() )
                    {
                        itKeyframes = timestampsByBlock.insert(
                                            std::make_pair( (size_t)blockIdx, emptyVec ) ).first;
                    }

                    itKeyframes->second.reserve(track->GetNumFrames());

                    for (size_t i = 0; i<track->GetNumFrames(); ++i)
                    {
                        float timestamp = track->GetKeyFrame(i)->GetTime();
                        TimestampVec::iterator it = std::lower_bound( itKeyframes->second.begin(),
                                                                        itKeyframes->second.end(),
                                                                        timestamp );
                        if( it == itKeyframes->second.end() || *it != timestamp )
                            itKeyframes->second.insert( it, timestamp );
                    }

                    size_t trackDiff = std::distance( timestampsByBlock.begin(), itKeyframes );
                    mBoneToWeights[skeleton->GetBone( boneIdx )->GetName()] =
                                        (boneToSlot[boneIdx] & 0xFF000000) |
                                        ((trackDiff * ARRAY_PACKED_REALS + slotIdx) & 0x00FFFFFF);
                }
            }

            //Now that we've built the list of unique keyframes, allocate the mTracks and its keyframes
            allocateCacheFriendlyKeyframes( timestampsByBlock, frameRate );
        }

        //Set now all the transforms the allocated space
        SkeletonTrackVec::iterator itTrack = mTracks.begin();
        SkeletonTrackVec::iterator enTrack = mTracks.end();

        while( itTrack != enTrack )
        {
            KeyFrameRigVec &keyFrames = itTrack->_getKeyFrames();
            KeyFrameRigVec::iterator itKeys = keyFrames.begin();
            KeyFrameRigVec::iterator enKeys = keyFrames.end();

            uint32 blockIdx = itTrack->getBoneBlockIdx();
            uint32 slotStart= blockIdxToSlotStart( blockIdx );

            while( itKeys != enKeys )
            {
                float fTime = itKeys->mFrame * frameRate;

                for( uint32 i=0; i<ARRAY_PACKED_REALS; ++i )
                {
                    uint32 slotIdx = slotStart + i;

                    uint32 boneIdx = -1;
                    auto it = slotToBone.find( slotIdx );
                    if( it != slotToBone.end() )
                        boneIdx = it->second;

                    if( animation->HasNodeClip( boneIdx ) )
                    {
                        auto *oldTrack = animation->GetNodeClip( boneIdx );

                        DiTransformKeyFrame originalKF( 0, fTime );
                        oldTrack->GetInterpolatedKeyFrame( animation->GetTimeIndex( fTime ),
                                                            &originalKF );

                        itKeys->mBoneTransform->mPosition.setFromVector3( originalKF.GetTranslate(), i );
                        itKeys->mBoneTransform->mOrientation.setFromQuaternion( originalKF.GetRotation(),
                                                                                i );
                        itKeys->mBoneTransform->mScale.setFromVector3( originalKF.GetScale(), i );
                        itTrack->_setMaxUsedSlot( i );
                    }
                    else
                    {
                        //This bone is not affected at all by the animation, but is
                        //part of the block. Fill with identity transform
                        itKeys->mBoneTransform->mPosition.setFromVector3( DiVec3::ZERO, i );
                        itKeys->mBoneTransform->mOrientation.setFromQuaternion( DiQuat::IDENTITY, i );
                        itKeys->mBoneTransform->mScale.setFromVector3( DiVec3::UNIT_SCALE, i );
                    }
                }

                ++itKeys;
            }

            itTrack->_bakeUnusedSlots();

            ++itTrack;
        }
    }
    
    void SkeletonAnimationDef::allocateCacheFriendlyKeyframes(
                                            const TimestampsPerBlock &timestampsByBlock, float frameRate )
    {
        assert( !mKfTransformMemoryManager );

        size_t numKeyFrames = 0;
        TimestampsPerBlock::const_iterator itor = timestampsByBlock.begin();
        TimestampsPerBlock::const_iterator end  = timestampsByBlock.end();

        while( itor != end )
        {
            numKeyFrames += itor->second.size();
            ++itor;
        }

        mKfTransformMemoryManager = new KfTransformArrayMemoryManager(
                                            0, numKeyFrames * ARRAY_PACKED_REALS,
                                            -1, numKeyFrames * ARRAY_PACKED_REALS );
        mKfTransformMemoryManager->initialize();

        mTracks.reserve( timestampsByBlock.size() );

        //1st pass: All first keyframes.
        itor = timestampsByBlock.begin();
        while( itor != end )
        {
            size_t blockIdx = itor->first;
            mTracks.push_back( SkeletonTrack( static_cast<uint32>(blockIdx), mKfTransformMemoryManager ) );

            mTracks.back().addKeyFrame( *itor->second.begin(), frameRate );
            ++itor;
        }

        DiVector<size_t> keyframesDone; //One per block
        keyframesDone.resize( timestampsByBlock.size(), 1 );

        //2nd pass: The in-between keyframes.
        bool addedAny = true;
        while( addedAny )
        {
            addedAny = false;

            //Keep adding keyframes until we're done with them
            size_t i = 0;
            itor = timestampsByBlock.begin();
            while( itor != end )
            {
                if( keyframesDone[i] < itor->second.size() - 1 )
                {
                    //First find if there are key frames (J0) from the next blocks whose
                    //next keyframe (J1) is before ours (I0), and add those instead (J0).
                    //If not, add ours.
                    float timestampI0 = itor->second[keyframesDone[i]];

                    bool subAddedAny = false;
                    size_t j = i+1;
                    TimestampsPerBlock::const_iterator itor2 = itor;
                    ++itor2;
                    while( itor2 != end )
                    {
                        if( keyframesDone[j] < itor2->second.size() - 1 )
                        {
                            float timestampJ1 = itor2->second[keyframesDone[j] + 1];
                            if( timestampJ1 <= timestampI0 )
                            {
                                float timestampJ0 = itor2->second[keyframesDone[j]];
                                mTracks[j].addKeyFrame( timestampJ0, frameRate );
                                ++keyframesDone[j];
                                addedAny = subAddedAny = true;
                            }
                        }

                        ++j;
                        ++itor2;
                    }

                    //Don't add ours if a previous track's keyframe (H0) is between
                    //our keyframe I0 and our next keyframe I1. We'll add both together
                    //in the next iteration
                    bool dontAdd = false;
                    float timestampI1 = itor->second[keyframesDone[i] + 1];
                    j = 0;
                    itor2 = timestampsByBlock.begin();
                    while( itor2 != itor && !dontAdd )
                    {
                        if( keyframesDone[j] < itor2->second.size() - 1 )
                        {
                            float timestampJ0 = itor2->second[keyframesDone[j]];
                            if( timestampI1 > timestampJ0 && timestampI0 < timestampJ0 )
                            {
                                dontAdd = true;
                                addedAny = true; //There still a reason to iterate again
                            }
                        }
                        ++j;
                        ++itor2;
                    }

                    if( !subAddedAny && !dontAdd )
                    {
                        mTracks[i].addKeyFrame( timestampI0, frameRate );
                        ++keyframesDone[i];
                        addedAny = true;
                    }
                }

                ++i;
                ++itor;
            }
        }

        //3rd Pass: Create the last key frames
        size_t i = 0;
        itor = timestampsByBlock.begin();
        while( itor != end )
        {
            if( itor->second.size() > 1 )
            {
                mTracks[i].addKeyFrame( *(itor->second.end()-1), frameRate );
                ++keyframesDone[i];
            }
            ++i;
            ++itor;
        }
    }
}

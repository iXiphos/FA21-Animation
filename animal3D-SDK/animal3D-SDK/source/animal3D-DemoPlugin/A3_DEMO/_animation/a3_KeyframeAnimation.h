/*
	Copyright 2011-2020 Daniel S. Buckstein

	Licensed under the Apache License, Version 2.0 (the "License");
	you may not use this file except in compliance with the License.
	You may obtain a copy of the License at

		http://www.apache.org/licenses/LICENSE-2.0

	Unless required by applicable law or agreed to in writing, software
	distributed under the License is distributed on an "AS IS" BASIS,
	WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
	See the License for the specific language governing permissions and
	limitations under the License.
*/

/*
	animal3D SDK: Minimal 3D Animation Framework
	By Daniel S. Buckstein
	
	a3_KeyframeAnimation.h
	Data structures for fundamental keyframe animation.
*/

#ifndef __ANIMAL3D_KEYFRAMEANIMATION_H
#define __ANIMAL3D_KEYFRAMEANIMATION_H


#include "animal3D-A3DM/a3math/a3vector.h"
#include "animal3D-A3DM/a3math/a3interpolation.h"



//-----------------------------------------------------------------------------

#ifdef __cplusplus
extern "C"
{
#else	// !__cplusplus
typedef struct a3_Sample					a3_Sample;
typedef struct a3_Keyframe					a3_Keyframe;
typedef struct a3_KeyframePool				a3_KeyframePool;
typedef struct a3_Clip						a3_Clip;
typedef struct a3_ClipPool					a3_ClipPool;
typedef struct a3_ClipTransition			a3_ClipTransition;
typedef enum a3_clipTransitionType         a3_clipTransitionType;
#endif	// __cplusplus


//-----------------------------------------------------------------------------

// constant values
enum
{
	a3keyframeAnimation_nameLenMax = 32,
};


enum a3_clipTransitionType {
	a3_clipTransitionTypePause,
	a3_clipTransitionTypeForward,
	a3_clipTransitionTypeForwardPause,

	a3_clipTransitionTypeReverse,
	a3_clipTransitionTypeReversePause,
	a3_clipTransitionTypeForwardPlayBack,

	a3_clipTransitionTypeForwardPauseFirstFrame,
	a3_clipTransitionTypeReversePlayBack,
	a3_clipTransitionTypeReversePauseLastFrame,

};

// clip transitions
struct a3_ClipTransition {
	
	struct {
		// should the clip play reversed? 
		a3i8 reverse : 1;
		// should the clip begin playing?
		a3i8 playing : 1;

		// where the clip should being playing, 0 for start 1 for end 
		a3i8 clipStart : 1;

		// where in the frame it should start (frame param) 0 or 1
		a3i8 frameStart : 1;
	};

	// index of the clip to transition to
	a3ui32 index;

	a3_clipTransitionType transition;

	// pool where the clip is
	//a3_ClipPool* pool; // can a clip transition to a clip with a different pool?
};

// single generic value at time
struct a3_Sample
{
	a3real time; // (the x axis)
	a3real value; // (the y value)
};

// description of single keyframe
// metaphor: interval
struct a3_Keyframe
{
	// index in keyframe pool
	a3ui32 index;

	// interval of time for which this keyframe is active; cannot be zero
	a3f32 duration;

	// reciprocal of duration
	a3f32 durationInv;

	a3_Sample sample;
};


// pool of keyframe descriptors
struct a3_KeyframePool
{
	// array of keyframes, DO NOT STORE POINTERS TO CONTENTS
	a3_Keyframe *keyframes;

	// number of keyframes
	a3ui32 count;

	// total number of keyframes
	a3ui32 capacity;
};



// allocate keyframe pool
a3i32 a3keyframePoolCreate(a3_KeyframePool* keyframePool_out, const a3ui32 capacity);

// release keyframe pool
a3i32 a3keyframePoolRelease(a3_KeyframePool* keyframePool);

// initialize keyframe
a3i32 a3keyframeInit(a3_Keyframe* keyframe_out, const a3real duration, const a3ui32 value_x);

a3_Keyframe* a3keyframePoolGetAtIndex(a3_KeyframePool* pool, const a3ui32 index);

// "allocate" space for count number of frames, retuns the index of the first one
a3i32 a3keyframePoolNewFrames(a3_KeyframePool* keyframePool, const a3ui32 count);

//-----------------------------------------------------------------------------

// description of single clip
// metaphor: timeline
struct a3_Clip
{
	// clip name
	a3byte name[a3keyframeAnimation_nameLenMax];

	// index in clip pool
	a3ui32 index;

	// duration of clip
	a3f32 duration;

	// reciprocal of duration
	a3f32 durationInv;

	// number of keyframes referenced by clip (including first and last)
	a3ui32 count;

	// index of first keyframe in pool referenced by clip
	a3ui32 firstKeyframe;

	a3_ClipTransition transitionForward, transitionBackwards;
	
	// index of final keyframe in pool referenced by clip
	a3ui32 lastKeyframe;

	// pool of keyframes containing those included in the set
	const a3_KeyframePool* pool;
};

// group of clips
struct a3_ClipPool
{
	// array of clips
	a3_Clip* clips;

	// number of clips
	a3ui32 count;

	a3ui32 capacity;
};


// allocate clip pool
a3i32 a3clipPoolCreate(a3_ClipPool* clipPool_out, const a3ui32 count);

// release clip pool
a3i32 a3clipPoolRelease(a3_ClipPool* clipPool);

// "allocates" a new clip within the clip pool, returns index
a3ui32 a3clipPoolNewClip(a3_ClipPool* clipPool);

// initialize clip with first and last indices
a3i32 a3clipInit(a3_Clip* clip_out, const a3byte clipName[a3keyframeAnimation_nameLenMax], const a3_KeyframePool* keyframePool, const a3ui32 firstKeyframeIndex, const a3ui32 finalKeyframeIndex);

a3i32 a3clipInitDuration(a3_Clip* clip_out, const a3byte clipName[a3keyframeAnimation_nameLenMax], const a3_KeyframePool* keyframePool, const a3real duration, const a3ui32 firstKeyframeIndex, const a3ui32 finalKeyframeIndex);

// get clip index from pool
a3i32 a3clipGetIndexInPool(const a3_ClipPool* clipPool, const a3byte clipName[a3keyframeAnimation_nameLenMax]);

// calculate clip duration as sum of keyframes' durations
a3i32 a3clipCalculateDuration(a3_Clip* clip);

// calculate keyframes' durations by distributing clip's duration
a3i32 a3clipDistributeDuration(a3_Clip* clip, const a3real newClipDuration);


// return keyframe from timestamp, only works within 0 to clip duration
// returns index
a3i32 a3clipGetKeyframeFromTime(a3_Clip* clip, a3real time, a3real* keyframeTime_out, a3_Keyframe** keyframe_out);

a3i32 a3clipPoolLoadFromFile(a3_ClipPool* clipPool, a3_KeyframePool* keyPool, const char* path);
//-----------------------------------------------------------------------------


#ifdef __cplusplus
}
#endif	// __cplusplus


#include "_inl/a3_KeyframeAnimation.inl"


#endif	// !__ANIMAL3D_KEYFRAMEANIMATION_H
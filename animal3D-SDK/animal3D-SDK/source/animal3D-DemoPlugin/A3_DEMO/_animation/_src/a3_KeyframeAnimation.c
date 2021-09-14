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
	
	a3_KeyframeAnimation.c
	Implementation of keyframe animation interfaces.
*/

#include "../a3_KeyframeAnimation.h"
#include "../../_a3_demo_utilities/a3_DemoUtil.h"

#include <stdlib.h>
#include <string.h>


// macros to help with names
#define A3_CLIP_DEFAULTNAME		("unnamed clip")
#define A3_CLIP_SEARCHNAME		((clipName && *clipName) ? clipName : A3_CLIP_DEFAULTNAME)


//-----------------------------------------------------------------------------

// allocate keyframe pool
a3i32 a3keyframePoolCreate(a3_KeyframePool* keyframePool_out, const a3ui32 count)
{
	keyframePool_out->keyframes = (a3_Keyframe*)malloc(sizeof(a3_Keyframe) * count);
	keyframePool_out->count = count;
	return 1;
}

// release keyframe pool
a3i32 a3keyframePoolRelease(a3_KeyframePool* keyframePool)
{
	free(keyframePool->keyframes);
	return 1;
}

// initialize keyframe
a3i32 a3keyframeInit(a3_Keyframe* keyframe_out, const a3real duration, const a3ui32 value_x)
{
	keyframe_out->sample.value = (float)value_x;
	keyframe_out->duration = duration;
	keyframe_out->durationInv = 1/duration;
	return 1;
}


a3i32 a3keyframePoolNewFrames(a3_KeyframePool* keyframePool, const a3ui32 count) {
	// if there is not enough room in the pool
	if ( keyframePool->count >= keyframePool->capacity ) {
		// keep growing until there is enough room
		do {
			keyframePool->capacity = (keyframePool->capacity + 1) * 2; // TODO: maybe different growth factor
		}  while ((keyframePool->capacity - count) < keyframePool->count);

		// resize keyframe array
		a3ResizeArray(keyframePool->keyframes, keyframePool->capacity, a3_Keyframe);
	}

	return keyframePool->count++;
}

// allocate clip pool
a3i32 a3clipPoolCreate(a3_ClipPool* clipPool_out, const a3ui32 count)
{
	clipPool_out->clips = (a3_Clip*)malloc(sizeof(a3_Clip) * count);
	clipPool_out->count = count;
	clipPool_out->capacity = count;
	return 1;
}

// release clip pool
a3i32 a3clipPoolRelease(a3_ClipPool* clipPool)
{
	free(clipPool);
	return 1;
}

a3i32 a3clipPoolNewClip(a3_ClipPool* clipPool) {
	if ( clipPool->count <= clipPool->capacity ) {
		clipPool->capacity += 5; // TODO: maybe different growth factor
		a3ResizeArray(clipPool->clips, clipPool->capacity, a3_Clip);
	}

	return clipPool->count++;
}

// initialize clip with first and last indices
a3i32 a3clipInit(a3_Clip* clip_out, const a3byte clipName[a3keyframeAnimation_nameLenMax], const a3_KeyframePool* keyframePool, const a3ui32 firstKeyframeIndex, const a3ui32 finalKeyframeIndex)
{
	memcpy(clip_out->name, clipName, a3keyframeAnimation_nameLenMax);
	clip_out->pool = keyframePool;
	clip_out->firstKeyframe = firstKeyframeIndex;
	clip_out->lastKeyframe = finalKeyframeIndex;
	clip_out->count = finalKeyframeIndex - firstKeyframeIndex + 1;
	clip_out->index = 0;
	a3clipCalculateDuration(clip_out);
	return 1;
}

// get clip index from pool
a3i32 a3clipGetIndexInPool(const a3_ClipPool* clipPool, const a3byte clipName[a3keyframeAnimation_nameLenMax])
{
	for (a3ui32 i = 0; i < clipPool->count; i++) {
		a3_Clip* clip = clipPool->clips + i;
		if ( 0 == memcmp(clip->name, clipName, a3keyframeAnimation_nameLenMax))
			return i;
	}
	return -1;
}

a3i32 a3clipPoolLoadFromFile(a3_ClipPool* clipPool, const char* path) {

	return -1;
}

//-----------------------------------------------------------------------------

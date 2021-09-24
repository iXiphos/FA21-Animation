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
#include "../../_a3_demo_utilities/a3_DemoUtils.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

// macros to help with names
#define A3_CLIP_DEFAULTNAME		("unnamed clip")
#define A3_CLIP_SEARCHNAME		((clipName && *clipName) ? clipName : A3_CLIP_DEFAULTNAME)


//-----------------------------------------------------------------------------

// allocate keyframe pool
a3i32 a3keyframePoolCreate(a3_KeyframePool* keyframePool_out, const a3ui32 count)
{
	a3AllocArray(keyframePool_out->keyframes, count, a3_Keyframe);
	keyframePool_out->count = count;
	keyframePool_out->capacity = count;
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
	free(clipPool->clips);
	return 1;
}

a3ui32 a3clipPoolNewClip(a3_ClipPool* clipPool) {
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
	clip_out->transitionForward.transition = a3_clipTransitionTypePause;
	clip_out->transitionBackwards.transition = a3_clipTransitionTypePause;
	clip_out->transitionForward.index = 0;
	clip_out->transitionBackwards.index = 0;
	a3clipCalculateDuration(clip_out);
	return 1;
}

// initialize clip with first and last indices
a3i32 a3clipInitDuration(a3_Clip* clip_out, const a3byte clipName[a3keyframeAnimation_nameLenMax], const a3_KeyframePool* keyframePool, const a3real duration, const a3ui32 firstKeyframeIndex, const a3ui32 finalKeyframeIndex)
{
	memcpy(clip_out->name, clipName, a3keyframeAnimation_nameLenMax);
	clip_out->pool = keyframePool;
	clip_out->firstKeyframe = firstKeyframeIndex;
	clip_out->lastKeyframe = finalKeyframeIndex;
	clip_out->count = finalKeyframeIndex - firstKeyframeIndex + 1;
	clip_out->index = 0;
	a3clipDistributeDuration(clip_out, duration);
	return 1;
}


// get clip index from pool
a3i32 a3clipGetIndexInPool(const a3_ClipPool* clipPool, const a3byte clipName[a3keyframeAnimation_nameLenMax])
{
	for (a3ui32 i = 0; i < clipPool->count; i++) {
		a3_Clip* clip = clipPool->clips + i;
		if ( 0 == strcmp(clip->name, clipName))
			return i;
	}
	return -1;
}

/*
a3_clipTransitionTypePause,
a3_clipTransitionTypeForward,
a3_clipTransitionTypeForwardPause,

a3_clipTransitionTypeReverse,
a3_clipTransitionTypeReversePause,
a3_clipTransitionTypeForwardPlayBack,

a3_clipTransitionTypeForwardPauseFirstFrame,
a3_clipTransitionTypeReversePlayBack,
a3_clipTransitionTypeReversePauseFirstFrame,
*/

const char* transitionStrs[9] = {
	"|",
	">",
	">|",
	"<",
	"<|",
	">>",
	">>|",
	"<<",
	"<<|",
};

a3_clipTransitionType parseTransitionMode(char* transTypeStr) {


	for (a3ui32 i = 0; i < 9; i++) {
		if (strcmp(transitionStrs[i], transTypeStr) == 0) 
			return a3_clipTransitionTypePause + i;
	}
	return -1;
}

a3i32 a3clipPoolLoadFromFile(a3_ClipPool* clipPool, a3_KeyframePool* keyPool, const char* path) {

	/*
	char name[a3keyframeAnimation_nameLenMax + 4];
	char reverse_transition[a3keyframeAnimation_nameLenMax + 4],
		 forward_transition[a3keyframeAnimation_nameLenMax + 4];
	char reverse_dir[2], forward_dir[2];
	float duration;
	a3ui32 first_frame, last_frame;
	*/

	// columns
	// clip_name	duration_s	first_frame	last_frame	reverse_transition	forward_transition

	char* lines[256];
	a3i32 line_count = a3ReadLinesFromFile(path, lines, 256);
	if ( line_count == -1) printf("unable to open file %s \n", path);
	printf("line_count = %i \n", line_count);

	a3ui32 clipIndexStart = clipPool->count;
	char* data_columns[256][7];
	a3ui32 newClipCount = 0;
	for (a3i32 i = 0; i < line_count; i++) {
		if (*lines[i] != '@') continue;
		a3ui32 column_num = a3SplitString(lines[i] + 2, '\t', data_columns[newClipCount], 10, true);
		if (column_num > 7 || column_num < 6) printf("error reading file\n");
		newClipCount++;
	}


	for (a3ui32 i = 0; i < newClipCount; i++) {

		char** columns = data_columns[i];

		a3i32 index = a3clipPoolNewClip(clipPool);
		a3_Clip* temp = clipPool->clips + index;
	
		a3real duration = (a3real)atof(columns[1]);
		a3ui32 firstFrame = atoi(columns[2]);
		a3ui32 lastFrame = atoi(columns[3]);

		a3clipInit(temp, columns[0], keyPool, firstFrame, lastFrame);
		
		

	}

	// deal with transitions
	for (a3ui32 i = 0; i < newClipCount; i++) {
		char** columns = data_columns[i];
		a3_Clip* clip = clipPool->clips + clipIndexStart + i;

		a3i32 col;


		char* rTransition[2];
		col = a3SplitString(columns[4], ' ', rTransition, 2, true);
		clip->transitionBackwards.transition = parseTransitionMode(rTransition[0]);
		if (col == 2) {
			a3i32 clipIndex = a3clipGetIndexInPool(clipPool, rTransition[1]);
			if (clipIndex == -1) printf("unable to find clip\n");
			clip->transitionBackwards.index = clipIndex;
			
		}


		
		char* fTransition[2];
		col = a3SplitString(columns[5], ' ', fTransition, 2, true);
		clip->transitionForward.transition = parseTransitionMode(fTransition[0]);
		if (col == 2) {
			a3i32 clipIndex = a3clipGetIndexInPool(clipPool, fTransition[1]);
			if (clipIndex == -1) printf("unable to find clip\n");
			clip->transitionForward.index = clipIndex;
		}
	}

	free(lines[0]);
	return -1;
}

//-----------------------------------------------------------------------------

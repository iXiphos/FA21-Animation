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

a3i32 a3channelPoolCreate(a3_ChannelPool* channelPool_out, const a3ui32 capacity)
{
	a3AllocArray(channelPool_out->channels, capacity, a3_channel);
	a3AllocArray(channelPool_out->samples, capacity, a3_Sampler);
	channelPool_out->count = capacity;
	channelPool_out->capacity = capacity;

	return 1;
}

a3i32 a3channelPoolRelease(a3_ChannelPool* channelPool)
{
	free(channelPool->channels);
	free(channelPool->samples);
	return 1;
}

a3i32 a3channelInit(a3_channel* channel_out, const a3ui32 sampleIndex, const a3_path path)
{
	channel_out->samplerIndex = sampleIndex;
	channel_out->targetPath = path;
	return 1;
}

a3i32 a3samplerInit(a3_Sampler* sampler_out, const a3real value_start, const a3real time_start, const a3real value_end, const a3real time_end, const a3_interpolationType interType)
{
	sampler_out->input.value = value_start;
	sampler_out->input.time = time_start;
	sampler_out->output.value = value_end;
	sampler_out->output.time = time_end;
	sampler_out->interpType = interType;
	return 1;
}

a3i32 a3accessorsInitFloat(a3_Accessors* accessors_out, const a3ui32 bufferView, const a3ui32 byteOffset, const a3ui32 componentType, const a3ui32 count, const a3f32 min, const a3f32 max)
{
	accessors_out->accessorType = a3_scalarAccessor;
	accessors_out->bufferView = bufferView;
	accessors_out->byteOffset = byteOffset;
	accessors_out->componentType = componentType;
	accessors_out->count = count;
	accessors_out->fmin = min;
	accessors_out->fmax = max;
	return 1;
}

a3i32 a3accessorsInitVec3(a3_Accessors* accessors_out, const a3ui32 bufferView, const a3ui32 byteOffset, const a3ui32 componentType, const a3ui32 count, const a3vec3 min, a3vec3 max)
{
	accessors_out->accessorType = a3_vec3Accessor;
	accessors_out->bufferView = bufferView;
	accessors_out->byteOffset = byteOffset;
	accessors_out->componentType = componentType;
	accessors_out->count = count;
	accessors_out->vec3min = min;
	accessors_out->vec3max = max;
	return 1;
}

a3i32 a3accessorsInitVec4(a3_Accessors* accessors_out, const a3ui32 bufferView, const a3ui32 byteOffset, const a3ui32 componentType, const a3ui32 count, const a3vec4 min, a3vec4 max)
{
	accessors_out->accessorType = a3_vec4Accessor;
	accessors_out->bufferView = bufferView;
	accessors_out->byteOffset = byteOffset;
	accessors_out->componentType = componentType;
	accessors_out->count = count;
	accessors_out->vec4min = min;
	accessors_out->vec4max = max;
	return 1;
}

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
a3i32 a3keyframeInit(a3_Keyframe* keyframe_out, const a3real duration, const a3real value_x)
{
	keyframe_out->sample.value = value_x;
	keyframe_out->duration = duration;
	keyframe_out->durationInv = 1/duration;
	return 1;
}

a3i32 a3keyFramePoolFromChannelPool(a3_KeyframePool* keyframePool_out, const a3_ChannelPool* channelPool_in)
{
	a3keyframePoolCreate(keyframePool_out, channelPool_in->capacity);
	a3f32 duration;
	a3_Sampler* tempSamp = channelPool_in->samples;
	a3_channel* tempChannel = channelPool_in->channels;
	for (a3ui32 i = 0; i < channelPool_in->capacity; i++) 
	{
		duration = tempSamp[tempChannel[i].samplerIndex].output.time - tempSamp[tempChannel[i].samplerIndex].input.time;
		a3keyframeInit(keyframePool_out->keyframes + i, duration, (float)tempSamp[tempChannel[i].samplerIndex].output.value);
		keyframePool_out->accessors = channelPool_in->accessors;
	}

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

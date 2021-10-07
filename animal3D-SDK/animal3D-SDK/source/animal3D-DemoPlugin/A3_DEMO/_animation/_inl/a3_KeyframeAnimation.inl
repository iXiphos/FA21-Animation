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
	
	a3_KeyframeAnimation.inl
	Inline definitions for keyframe animation.
*/

#ifdef __ANIMAL3D_KEYFRAMEANIMATION_H
#ifndef __ANIMAL3D_KEYFRAMEANIMATION_INL
#define __ANIMAL3D_KEYFRAMEANIMATION_INL


//-----------------------------------------------------------------------------

// calculate clip duration as sum of keyframes' durations
inline a3i32 a3clipCalculateDuration(a3_Clip* clip)
{
	a3_Keyframe* keyframes = clip->pool->keyframes;

	float duration = 0;
	for (a3ui32 i = clip->firstKeyframe; i <= clip->lastKeyframe; i++) {
		duration += keyframes[i].duration;
	}
	
	clip->duration = duration;
	clip->durationInv = duration == 0 ? 0 : 1 / duration;

	return 1;
}

// calculate keyframes' durations by distributing clip's duration
inline a3i32 a3clipDistributeDuration(a3_Clip* clip, const a3real newClipDuration)
{
	a3_Keyframe* keyframes = clip->pool->keyframes;
	clip->duration = newClipDuration;
	clip->durationInv = newClipDuration == 0 ? 0 : 1 / newClipDuration;
	

	float frame_duration = clip->count * clip->durationInv;
	for (a3ui32 i = clip->firstKeyframe; i <= clip->lastKeyframe; i++) {
		keyframes[i].duration = frame_duration;
	}

	return 1;
}


//-----------------------------------------------------------------------------


#endif	// !__ANIMAL3D_KEYFRAMEANIMATION_INL
#endif	// __ANIMAL3D_KEYFRAMEANIMATION_H
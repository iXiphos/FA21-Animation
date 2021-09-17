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
	
	a3_KeyframeAnimationController.h
	inline definitions for keyframe animation controller.
*/

#ifdef __ANIMAL3D_KEYFRAMEANIMATIONCONTROLLER_H
#ifndef __ANIMAL3D_KEYFRAMEANIMATIONCONTROLLER_INL
#define __ANIMAL3D_KEYFRAMEANIMATIONCONTROLLER_INL


//-----------------------------------------------------------------------------

// update clip controller
inline a3i32 a3clipControllerUpdate(a3_ClipController* clipCtrl, const a3real dt)
{
	// case reversed terminus
	// case reversed skip
	// case reversed
	// case paused
	// case forward
	// case forward skip
	// case forward terminus


	// case paused
	if (clipCtrl->playing == 0) return 0;

	a3_Clip* clip = clipCtrl->clipPool->clips + clipCtrl->clip;
	a3_Keyframe* last_keyframe;
	

	last_keyframe = clip->pool->keyframes + clipCtrl->keyframe;

	float direction = clipCtrl->reverse ? -1.0F : 1.0F;
	clipCtrl->keyframeTime += dt * direction;
	clipCtrl->clipTime += dt * direction;



	// end of clip forward
	if (clipCtrl->clipTime > clip->duration) {
		// TODO "termination behavior"
		// maintain extra duration from end
		float extraTime = clipCtrl->clipTime - clip->duration;
		clipCtrl->clipTime = 0 + extraTime;
		clipCtrl->keyframeTime = 0 + extraTime;
		clipCtrl->keyframe = clip->firstKeyframe;
	}
	// end of clip reversed
	else if (clipCtrl->clipTime < 0) {
		a3_Keyframe* next_keyframe = clip->pool->keyframes + clip->lastKeyframe;
		float extraTime = -clipCtrl->clipTime;
		clipCtrl->clipTime = clip->duration - extraTime;
		clipCtrl->keyframeTime = next_keyframe->duration - extraTime;
		clipCtrl->keyframe = clip->lastKeyframe;
	}
	// because of the checks before, we know that the keyframe will be inside the current clip
	// forward skip
	else if (clipCtrl->keyframeTime > last_keyframe->duration)
	{
		clipCtrl->keyframeTime = clipCtrl->keyframeTime - last_keyframe->duration;
		clipCtrl->keyframe++;
	}	
	// reverse skip
	else if (clipCtrl->keyframeTime < 0) {
		clipCtrl->keyframe--;
		a3_Keyframe* next_keyframe = clip->pool->keyframes + clipCtrl->keyframe;
		clipCtrl->keyframeTime = next_keyframe->duration - -clipCtrl->keyframeTime;
		
	}

	// case forward and
	// case reverse
	a3_Keyframe* current_keyframe = clip->pool->keyframes + clipCtrl->keyframe;
	clipCtrl->clipParam = clipCtrl->clipTime * clip->durationInv;
	clipCtrl->keyframeParam = clipCtrl->keyframeTime * current_keyframe->durationInv;

	return 1;
}

// set clip to play
inline a3i32 a3clipControllerSetClip(a3_ClipController* clipCtrl, const a3_ClipPool* clipPool, const a3ui32 clipIndex_pool)
{
	a3_Clip* clip = clipPool->clips + clipIndex_pool;
	clipCtrl->clip = clipIndex_pool;
	clipCtrl->clipPool = clipPool;
	// if we are playing reverse: 
	// start at end of time, set keyframe to last keyframe and set keyframe param to 1
	clipCtrl->keyframe = clipCtrl->reverse ? clip->lastKeyframe : clip->lastKeyframe;
	a3_Keyframe* keyframe = clip->pool->keyframes + clipCtrl->keyframe;

	float param = (float)clipCtrl->reverse; // forward: 0, reverse: 1
	clipCtrl->clipParam = param;
	clipCtrl->keyframeParam = param;
	clipCtrl->clipTime = param * clip->duration;
	clipCtrl->keyframeTime = 0;

	return 1;
}



inline a3real a3clipControllerEvaulate(const a3_ClipController* clipCtrl, a3_Sample* sample_out) {
	if (clipCtrl && clipCtrl->clip != -1 && sample_out) {
		a3_Clip* clip = clipCtrl->clipPool->clips + clipCtrl->clip;
		// 0: no interpolation: step
		// return keyframe value
		return clip->pool->keyframes[clipCtrl->keyframe].sample.value;
		// 1: nearest

		// 2: lerp


		// 3: 


		return 1;
	}
	return -1;
}


//-----------------------------------------------------------------------------


#endif	// !__ANIMAL3D_KEYFRAMEANIMATIONCONTROLLER_INL
#endif	// __ANIMAL3D_KEYFRAMEANIMATIONCONTROLLER_H
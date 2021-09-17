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


inline a3i32 a3clipControllerHandleTransition(a3_ClipController* clipCtrl) {
	a3_Clip* from_clip = clipCtrl->clipPool->clips + clipCtrl->clip;

	
	//update keyframe, cliptime, direction, paused, keyframe time

	float extraTime = 0.0f;
	a3_Keyframe firstKeyFrame = from_clip->pool->keyframes[from_clip->firstKeyframe];
	a3_Keyframe lastKeyFrame = from_clip->pool->keyframes[from_clip->lastKeyframe];

	a3_ClipTransition transition = clipCtrl->reverse ? from_clip->transitionBackwards : from_clip->transitionForward;

	switch (transition.transition) 
	{
	case a3_clipTransitionTypePause:
		clipCtrl->clipTime = from_clip->duration * (1 - clipCtrl->reverse);
		clipCtrl->keyframeTime = from_clip->pool->keyframes[clipCtrl->keyframe].duration;

		clipCtrl->playing = false;
		break;
	case a3_clipTransitionTypeForward:
		extraTime = clipCtrl->clipTime - from_clip->duration;
		clipCtrl->clipTime = 0 + extraTime;
		clipCtrl->keyframeTime = 0 + extraTime;
		clipCtrl->keyframe = from_clip->firstKeyframe;

		clipCtrl->playing = true;
		clipCtrl->reverse = false;
		break;
	case a3_clipTransitionTypeForwardPause:
		extraTime = clipCtrl->clipTime - from_clip->duration;
		clipCtrl->clipTime = 0 + extraTime;
		clipCtrl->keyframeTime = 0 + extraTime;
		clipCtrl->keyframe = from_clip->firstKeyframe;

		clipCtrl->playing = false;
		clipCtrl->reverse = false;
		break;

	case a3_clipTransitionTypeReverse:
		extraTime = clipCtrl->clipTime - from_clip->duration;
		clipCtrl->clipTime = 0 + extraTime;
		clipCtrl->keyframeTime = 0 + extraTime;
		clipCtrl->keyframe = from_clip->lastKeyframe;

		clipCtrl->playing = false;
		clipCtrl->reverse = true;
		break;
	case a3_clipTransitionTypeReversePause:
		extraTime = clipCtrl->clipTime - from_clip->duration;
		clipCtrl->clipTime = 0 + extraTime;
		clipCtrl->keyframeTime = 0 + extraTime;
		clipCtrl->keyframe = from_clip->lastKeyframe;

		clipCtrl->playing = true;
		clipCtrl->reverse = true;
		break;
	case a3_clipTransitionTypeForwardPlayBack:
		extraTime = clipCtrl->clipTime - from_clip->duration;
		clipCtrl->clipTime = firstKeyFrame.duration + extraTime;
		clipCtrl->keyframeTime = firstKeyFrame.duration + extraTime;
		clipCtrl->keyframe = from_clip->firstKeyframe;

		clipCtrl->playing = true;
		clipCtrl->reverse = false;
		break;

	case a3_clipTransitionTypeForwardPauseFirstFrame:
		extraTime = clipCtrl->clipTime - from_clip->duration;
		clipCtrl->clipTime = firstKeyFrame.duration + extraTime;
		clipCtrl->keyframeTime = firstKeyFrame.duration + extraTime;
		clipCtrl->keyframe = from_clip->firstKeyframe;

		clipCtrl->playing = false;
		clipCtrl->reverse = false;
		break;
	case a3_clipTransitionTypeReversePlayBack:
		extraTime = clipCtrl->clipTime - from_clip->duration;
		clipCtrl->clipTime = lastKeyFrame.duration + extraTime;
		clipCtrl->keyframeTime = lastKeyFrame.duration + extraTime;
		clipCtrl->keyframe = from_clip->lastKeyframe;

		clipCtrl->playing = true;
		clipCtrl->reverse = true;
		break;
	case a3_clipTransitionTypeReversePauseLastFrame:
		extraTime = clipCtrl->clipTime - from_clip->duration;
		clipCtrl->clipTime = lastKeyFrame.duration + extraTime;
		clipCtrl->keyframeTime = lastKeyFrame.duration + extraTime;
		clipCtrl->keyframe = from_clip->lastKeyframe;

		clipCtrl->playing = false;
		clipCtrl->reverse = true;
		break;
	
	}
	return -1;
}

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



	// end of clip
	if (clipCtrl->clipTime < 0 || clipCtrl->clipTime > clip->duration) {

		a3clipControllerHandleTransition(clipCtrl);
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



inline a3i32 a3clipControllerEvaulate(const a3_ClipController* clipCtrl, a3_Sample* sample_out) {
	if (clipCtrl && clipCtrl->clip != -1 && sample_out) {
		a3_Clip* clip = clipCtrl->clipPool->clips + clipCtrl->clip;
		// 0: no interpolation: step
		// return keyframe value
		*sample_out = clip->pool->keyframes[clipCtrl->keyframe].sample;
		return 1;
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
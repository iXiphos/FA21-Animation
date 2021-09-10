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

	a3_Clip* clip;
	a3_Keyframe* last_keyframe;

	// case paused
	if (clipCtrl->direction == 0) return 0;
	clip = clipCtrl->clipPool->clip + clipCtrl->clip;
	last_keyframe = clip->pool->keyframe + clipCtrl->keyframe;
	
	clipCtrl->keyframeTime += dt * clipCtrl->direction;
	clipCtrl->clipTime += dt * clipCtrl->direction;


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
		float extraTime = clipCtrl->clipTime;
		clipCtrl->clipTime = clip->duration + extraTime;
		clipCtrl->keyframeTime = clip->duration + extraTime;
		clipCtrl->keyframe = clip->lastKeyframe;
	}
	// forward skip
	else if (clipCtrl->keyframeTime > last_keyframe->duration)
	{
		clipCtrl->keyframeTime = clipCtrl->keyframeTime - last_keyframe->duration;
		clipCtrl->keyframe++;
	}	
	// reverse skip
	else if (clipCtrl->keyframeTime < 0) {
		clipCtrl->keyframeTime = -clipCtrl->keyframeTime;
		clipCtrl->keyframe--;
	}

	// case forward and
	// case reverse
	a3_Keyframe* current_keyframe = clip->pool->keyframe + clipCtrl->keyframe;
	clipCtrl->clipParam = clipCtrl->clipTime * clip->durationInv;
	clipCtrl->keyframeParam = clipCtrl->keyframeTime * current_keyframe->durationInv;

	return 1;
}

// set clip to play
inline a3i32 a3clipControllerSetClip(a3_ClipController* clipCtrl, const a3_ClipPool* clipPool, const a3ui32 clipIndex_pool)
{
	clipCtrl->clip = clipIndex_pool;
	clipCtrl->clipPool = clipPool;
	return 1;
}


//-----------------------------------------------------------------------------


#endif	// !__ANIMAL3D_KEYFRAMEANIMATIONCONTROLLER_INL
#endif	// __ANIMAL3D_KEYFRAMEANIMATIONCONTROLLER_H
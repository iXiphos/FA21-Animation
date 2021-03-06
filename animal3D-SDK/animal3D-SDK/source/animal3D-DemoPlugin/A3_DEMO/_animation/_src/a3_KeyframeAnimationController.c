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
	
	a3_KeyframeAnimationController.c
	Implementation of keyframe animation controller.
*/

#include "../a3_KeyframeAnimationController.h"

#include <string.h>


//-----------------------------------------------------------------------------

// initialize clip controller
a3i32 a3clipControllerInit(a3_ClipController* clipCtrl_out, const a3byte ctrlName[a3keyframeAnimation_nameLenMax], const a3_ClipPool* clipPool, const a3ui32 clipIndex_pool)
{
	memcpy(clipCtrl_out->name, ctrlName, a3keyframeAnimation_nameLenMax);
	clipCtrl_out->clip = clipIndex_pool;
	clipCtrl_out->clipTime = 0;
	clipCtrl_out->keyframeTime = 0;
	clipCtrl_out->direction = 1;
	clipCtrl_out->keyframe = clipPool->clip[clipIndex_pool].firstKeyframe;
	clipCtrl_out->clipPool = clipPool;
	return 1;
}


a3i32 a3clipControllerJumpToFrame(a3_ClipController* clipCtrl, const a3ui32 index) {
	a3_Clip* clip = clipCtrl->clipPool->clip + clipCtrl->clip;
	if (index < clip->firstKeyframe || index > clip->lastKeyframe) return -1;

	float duration = 0;
	const a3_KeyframePool* pool = clip->pool;
	for (a3ui32 i = clip->firstKeyframe; i < index; i++) {
		duration += pool->keyframe[i].duration;
	}

	clipCtrl->keyframe = index;
	clipCtrl->clipTime = duration;
	clipCtrl->keyframeTime = 0;
	return 1;
}

//-----------------------------------------------------------------------------

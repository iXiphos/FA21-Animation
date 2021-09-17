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
	Keyframe animation clip controller. Basically a frame index manager. Very 
	limited in what one can do with this; could potentially be so much more.
*/

#ifndef __ANIMAL3D_KEYFRAMEANIMATIONCONTROLLER_H
#define __ANIMAL3D_KEYFRAMEANIMATIONCONTROLLER_H


#include "a3_KeyframeAnimation.h"


//-----------------------------------------------------------------------------

#ifdef __cplusplus
extern "C"
{
#else	// !__cplusplus
typedef struct a3_ClipController			a3_ClipController;
#endif	// __cplusplus


//-----------------------------------------------------------------------------

// clip controller
// metaphor: playhead
struct a3_ClipController
{
	a3byte name[a3keyframeAnimation_nameLenMax];

	// index of clip to control in referenced clip pool
	a3ui32 clip;

	// current time relative to start of clip; should always be between 0 and current clip's duration
	a3f32 clipTime;

	//  normalized keyframe time; should always be between 0 and 1
	a3f32 clipParam;

	// index of current keyframe in referenced keyframe pool 
	a3ui32 keyframe;

	// current time relative to current keyframe; should always be between 0 and current keyframe's duration
	a3f32 keyframeTime;

	// normalized keyframe time; should always be between 0 and 1.
	a3f32 keyframeParam;


	// is it playing reversed?
	a3i8 reverse;

	// boolean
	a3i8 playing;

	// the pool of clips that the controller will control
	const a3_ClipPool* clipPool;
};


//-----------------------------------------------------------------------------

// initialize clip controller
a3i32 a3clipControllerInit(a3_ClipController* clipCtrl_out, const a3byte ctrlName[a3keyframeAnimation_nameLenMax], const a3_ClipPool* clipPool, const a3ui32 clipIndex_pool);

// update clip controller
a3i32 a3clipControllerUpdate(a3_ClipController* clipCtrl, const a3real dt);

// set clip to play
a3i32 a3clipControllerSetClip(a3_ClipController* clipCtrl, const a3_ClipPool* clipPool, const a3ui32 clipIndex_pool);

a3i32 a3clipControllerGetClip(a3_ClipController* clipCtrl);

a3i32 a3clipControllerJumpToFrame(a3_ClipController* clipCtrl, const a3ui32 index);

a3i32 a3clipControllerEvaulate(const a3_ClipController* clipCtrl, a3_Sample* sample_out);


// get a frame based on an offset from the current frame
// will follow termination behavior and play direction
a3_Keyframe* a3clipControllerGetFrameByOffset(const a3_ClipController* clipCtrl, a3i32 offset);


//-----------------------------------------------------------------------------


#ifdef __cplusplus
}
#endif	// __cplusplus


#include "_inl/a3_KeyframeAnimationController.inl"


#endif	// !__ANIMAL3D_KEYFRAMEANIMATIONCONTROLLER_H
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
	
	a3_HierarchyStateBlend.h
	Hierarchy blend operations.
*/

#ifndef __ANIMAL3D_HIERARCHYSTATEBLEND_H
#define __ANIMAL3D_HIERARCHYSTATEBLEND_H


#include "a3_HierarchyState.h"

#include "a3_Kinematics.h"


#ifdef __cplusplus
extern "C"
{
#else	// !__cplusplus

#endif	// __cplusplus

//Blend operation function
/*
All blendable values in spatial pose is a a4vec4
example in load:
a3_SpatialPoseBlendOpLerp testLerp;
testLerp.opOrientation = a3vec4Nlerp;

resultAngles = testLerp.opOrientation(args)

*/

typedef a3vec4 (*a3_BlendOpLerp)(a3vec4 const v0, a3vec4 const v1, a3real const u);
typedef struct a3SpatialposeBlendOpLerp {
	a3_BlendOpLerp opOrientation, opAngles, opScale, opTranslation;
} a3SpatialposeBlendOpLerp;

inline a3vec4 a3vec4Lerp(a3vec4 const v0, a3vec4 const v1, a3real const u) {
	//implement linear interpolation 
	return v0;
}
inline a3vec4 a3vec4SLerp(a3vec4 const v0, a3vec4 const v1, a3real const u) {
	//implement spherical interpolation 
	return v0;
}
inline a3vec4 a3vec4NLerp(a3vec4 const v0, a3vec4 const v1, a3real const u) {
	//implement normal interpolation 
	return v0;
}
inline a3vec4 a3vec4LogLerp(a3vec4 const v0, a3vec4 const v1, a3real const u) {
	//implement log interpolation 
	return v0;
}

//-----------------------------------------------------------------------------

typedef struct a3_SpatialPoseBlendArgs {
	union {
		a3_SpatialPose poses[4];
		struct {
			a3_SpatialPose* pose0;
			a3_SpatialPose* pose1;
			a3_SpatialPose* pose2;
			a3_SpatialPose* pose3;
		};
	};
} a3_SpatialPoseBlendArgs;

typedef a3_SpatialPose* (*a3_SpatialPoseBlendOp)(a3_SpatialPose* pose_out, a3_SpatialPoseBlendArgs args, a3real const u);


// pointer-based reset/identity operation for single spatial pose
a3_SpatialPose* a3spatialPoseOpIdentity(a3_SpatialPose* pose_out);

// pointer-based LERP operation for single spatial pose
a3_SpatialPose* a3spatialPoseOpLERP(a3_SpatialPose* pose_out, a3_SpatialPose const* pose0, a3_SpatialPose const* pose1, a3real const u);


//-----------------------------------------------------------------------------

// data-based reset/identity
a3_SpatialPose a3spatialPoseDOpIdentity();

// data-based LERP
a3_SpatialPose a3spatialPoseDOpLERP(a3_SpatialPose const pose0, a3_SpatialPose const pose1, a3real const u);


//-----------------------------------------------------------------------------


struct a3_HierarchyPoseBlend {
	a3_SpatialPoseBlendOp* blend;
};

typedef a3_HierarchyPose* (*a3_HierarchyPoseBlendOp)(a3_HierarchyPose* pose_out, a3_HierarchyPose const* pose0, a3_HierarchyPose const* pose1, a3ui32 num_nodes, a3real const u);

// pointer-based reset/identity operation for hierarchical pose
a3_HierarchyPose* a3hierarchyPoseOpIdentity(a3_HierarchyPose* pose_out);

// pointer-based LERP operation for hierarchical pose
a3_HierarchyPose* a3hierarchyPoseOpLERP(a3_HierarchyPose* pose_out, a3_HierarchyPose const* pose0, a3_HierarchyPose const* pose1, a3real const u);



//-----------------------------------------------------------------------------


#ifdef __cplusplus
}
#endif	// __cplusplus


#include "_inl/a3_HierarchyStateBlend.inl"


#endif	// !__ANIMAL3D_HIERARCHYSTATEBLEND_H
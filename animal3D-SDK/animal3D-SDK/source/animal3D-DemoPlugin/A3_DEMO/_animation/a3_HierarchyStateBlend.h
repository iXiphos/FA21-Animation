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

#include <math.h>

#include "a3_HierarchyState.h"

#include "a3_Kinematics.h"


#ifdef __cplusplus
extern "C"
{
#else	// !__cplusplus

#endif	// __cplusplus

typedef a3vec4 (*a3_BlendOpLerp)(a3vec4 v0, a3vec4 v1, a3real const u);
typedef struct a3SpatialposeBlendOpLerp {
	a3_BlendOpLerp opOrientation, opAngles, opScale, opTranslation;
} a3SpatialposeBlendOpLerp;

inline a3vec4 a3vec4Lerp(a3vec4 v0, a3vec4 v1, a3real const u) {
	//implement linear interpolation 
	a3real4Sub(v1.v, v0.v);
	a3real4MulS(v1.v, u);
	a3real4Add(v0.v, v1.v);
	return v0;
}

inline a3vec4 a3vec4SLerp(a3vec4 v0, a3vec4 v1, a3real const u) {
	//implement spherical interpolation 


	return v0;
}
inline a3vec4 a3vec4NLerp(a3vec4 v0, a3vec4 v1, a3real const u) {
	//implement normal interpolation 
	a3real4Sub(v1.v, v0.v);
	a3real4MulS(v1.v, u);
	a3real4Add(v0.v, v1.v);
	a3real3Normalize(v0.v);
	return v0;
}
inline a3vec4 a3vec4LogLerp(a3vec4 v0, a3vec4 v1, a3real const u) {
	//implement log interpolation 

	a3real4DivComp(v1.v, v0.v);
	v1.x = powf(v1.x, u);
	v1.y = powf(v1.y, u);
	v1.z = powf(v1.z, u);

	a3real4MulComp(v0.v, v1.v);

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

	union {
		a3f64 params[5];
		struct {
			a3f64 param0;
			a3f64 param1;
			a3f64 param2;
			a3f64 param3;
			a3f64 param4;
		};
	};
} a3_SpatialPoseBlendArgs;

typedef a3_SpatialPose* (*a3_SpatialPoseBlendOp)(a3_SpatialPose* pose_out, a3_SpatialPoseBlendArgs args);

// pointer-based reset/identity operation for single spatial pose
a3_SpatialPose* a3spatialPoseOpIdentity(a3_SpatialPose* pose_out);

// pointer-based LERP operation for single spatial pose
a3_SpatialPose* a3spatialPoseOpLERP(a3_SpatialPose* pose_out, a3_SpatialPoseBlendArgs args);

a3_SpatialPose* a3spatialPoseOpConstruct(a3_SpatialPose* pose_out, a3vec4 orientation, a3vec4 scale, a3vec4 translation);

a3_SpatialPose* a3spatialPoseOpCopy(a3_SpatialPose* pose_out, a3_SpatialPoseBlendArgs args);

a3_SpatialPose* a3spatialPoseOpInvert(a3_SpatialPose* pose_out, a3_SpatialPoseBlendArgs args);

a3_SpatialPose* a3spatialPoseOpConcat(a3_SpatialPose* pose_out, a3_SpatialPoseBlendArgs args);

a3_SpatialPose* a3spatialPoseOpNearest(a3_SpatialPose* pose_out, a3_SpatialPoseBlendArgs args);

a3_SpatialPose* a3spatialPoseOpCubic(a3_SpatialPose* pose_out, a3_SpatialPoseBlendArgs args);

a3_SpatialPose* a3spatialPoseOpDeconcat(a3_SpatialPose* pose_out, a3_SpatialPoseBlendArgs args);

a3_SpatialPose* a3spatialPoseOpScale(a3_SpatialPose* pose_out, a3_SpatialPoseBlendArgs args);

a3_SpatialPose* a3spatialPoseOpTriangular(a3_SpatialPose* pose_out, a3_SpatialPoseBlendArgs args);

a3_SpatialPose* a3spatialPoseOpBiNearest(a3_SpatialPose* pose_out, a3_SpatialPoseBlendArgs args);

a3_SpatialPose* a3spatialPoseOpBiLinear(a3_SpatialPose* pose_out, a3_SpatialPoseBlendArgs args);

a3_SpatialPose* a3spatialPoseOpSmoothStep(a3_SpatialPose* pose_out, a3_SpatialPoseBlendArgs args);

a3_SpatialPose* a3spatialPoseOpDescale(a3_SpatialPose* pose_out, a3_SpatialPoseBlendArgs args);

a3_SpatialPose* a3spatialPoseOpConvert(a3_SpatialPose* pose_out, a3_SpatialPoseBlendArgs args);

a3_SpatialPose* a3spatialPoseOpRevert(a3_SpatialPose* pose_out, a3_SpatialPoseBlendArgs args);

a3_SpatialPose* a3spatialPoseOpBiCubic(a3_SpatialPose* pose_out, a3_SpatialPose * poses0[4], a3_SpatialPose * poses1[4], a3_SpatialPose * poses2[4], a3_SpatialPose * poses3[4], a3real u[5]);

//-----------------------------------------------------------------------------

// data-based reset/identity
a3_SpatialPose a3spatialPoseDOpIdentity();

// data-based LERP
a3_SpatialPose a3spatialPoseDOpLERP(a3_SpatialPose const pose0, a3_SpatialPose const pose1, a3real const u);



//-----------------------------------------------------------------------------


a3_HierarchyPose* a3hierarchyPoseOpIdentity(a3_HierarchyPose* pose_out, a3ui32 num_nodes);

a3_HierarchyPose* a3hierarchyPoseOpLERP(a3_HierarchyPose* pose_out, a3_HierarchyPose const* pose0, a3_HierarchyPose const* pose1, a3real const u, a3ui32 num_nodes);

a3_HierarchyPose* a3hierarchyPoseOpConstruct(a3_HierarchyPose* pose_out, a3vec4 orientation, a3vec4 scale, a3vec4 translation, a3ui32 num_nodes);

a3_HierarchyPose* a3hierarchyPoseOpCopy(a3_HierarchyPose* pose_out, a3_HierarchyPose const* pose, a3ui32 num_nodes);

a3_HierarchyPose* a3hierarchyPoseOpInvert(a3_HierarchyPose* pose_out, a3_HierarchyPose const* pose, a3ui32 num_nodes);

a3_HierarchyPose* a3hierarchyPoseOpConcat(a3_HierarchyPose* pose_out, a3_HierarchyPose const* pose0, a3_HierarchyPose const* pose1, a3ui32 num_nodes);

a3_HierarchyPose* a3hierarchyPoseOpNearest(a3_HierarchyPose* pose_out, a3_HierarchyPose const* pose0, a3_HierarchyPose const* pose1, a3real const u, a3ui32 num_nodes);

a3_HierarchyPose* a3hierarchyPoseOpCubic(a3_HierarchyPose* pose_out, a3_HierarchyPose const* pose0, a3_HierarchyPose const* pose1, a3_HierarchyPose const* pose2, a3_HierarchyPose const* pose3, a3real const u, a3ui32 num_nodes);

a3_HierarchyPose* a3hierarchyPoseOpDeconcat(a3_HierarchyPose* pose_out, a3_HierarchyPose const* pose0, a3_HierarchyPose const* pose1, a3ui32 num_nodes);

a3_HierarchyPose* a3hierarchyPoseOpScale(a3_HierarchyPose* pose_out, a3_HierarchyPose const* pose0, a3real const u, a3ui32 num_nodes);

a3_HierarchyPose* a3hierarchyPoseOpTriangular(a3_HierarchyPose* pose_out, a3_HierarchyPose const* pose0, a3_HierarchyPose const* pose1, a3_HierarchyPose const* pose2, a3real const u1, a3real const u2, a3ui32 num_nodes);

a3_HierarchyPose* a3hierarchyPoseOpBiNearest(a3_HierarchyPose* pose_out, a3_HierarchyPose const* pose0, a3_HierarchyPose const* pose1, a3_HierarchyPose const* pose2, a3_HierarchyPose const* pose3, a3real const u1, a3real const u2, a3real const u3, a3ui32 num_nodes);

a3_HierarchyPose* a3hierarchyPoseOpBiLinear(a3_HierarchyPose* pose_out, a3_HierarchyPose const* pose0, a3_HierarchyPose const* pose1, a3_HierarchyPose const* pose2, a3_HierarchyPose const* pose3, a3real const u1, a3real const u2, a3real const u3, a3ui32 num_nodes);

a3_HierarchyPose* a3hierarchyPoseOpBiCubic(a3_HierarchyPose* pose_out, a3_HierarchyPose* poses0[4], a3_HierarchyPose* poses1[4], a3_HierarchyPose* poses2[4], a3_HierarchyPose * poses3[4], a3real  u[5], a3ui32 num_nodes);

a3_HierarchyPose* a3hierarchyPoseOpSmoothLERP(a3_HierarchyPose* pose_out, a3_HierarchyPose const* pose0, a3_HierarchyPose const* pose1, a3real const u, a3ui32 num_nodes);

a3_HierarchyPose* a3hierarchyPoseOpDeScale(a3_HierarchyPose* pose_out, a3_HierarchyPose const* pose0, a3real const u, a3ui32 num_nodes);

a3_HierarchyPose* a3hierarchyPoseOpConvert(a3_HierarchyPose* pose_out, a3_HierarchyPose const* pose, a3ui32 num_nodes);

a3_HierarchyPose* a3hierarchyPoseOpRevert(a3_HierarchyPose* pose_out, a3_HierarchyPose const* pose, a3ui32 num_nodes);

a3_HierarchyPose* a3hierarchyPoseOKinematics(a3_HierarchyPose* pose_out, a3_Hierarchy const* hierarchy, a3_HierarchyPose const* poseObj, a3_HierarchyPose const* poseLoc);

a3_HierarchyPose* a3hierarchyPoseOpInverseKinematics(a3_HierarchyPose* pose_out, a3_Hierarchy const* hierarchy, a3_HierarchyPose const* poseObj, a3_HierarchyPose const* poseLoc);

//-----------------------------------------------------------------------------


#ifdef __cplusplus
}
#endif	// __cplusplus


#include "_inl/a3_HierarchyStateBlend.inl"


#endif	// !__ANIMAL3D_HIERARCHYSTATEBLEND_H
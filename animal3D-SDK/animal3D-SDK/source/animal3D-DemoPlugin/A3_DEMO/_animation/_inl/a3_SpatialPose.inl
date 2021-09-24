#include "..\a3_SpatialPose.h"
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
	
	a3_SpatialPose.inl
	Implementation of inline spatial pose operations.
*/


#ifdef __ANIMAL3D_SPATIALPOSE_H
#ifndef __ANIMAL3D_SPATIALPOSE_INL
#define __ANIMAL3D_SPATIALPOSE_INL


//-----------------------------------------------------------------------------

// set rotation values for a single node pose
inline a3i32 a3spatialPoseSetRotation(a3_SpatialPose* spatialPose, const a3f32 rx_degrees, const a3f32 ry_degrees, const a3f32 rz_degrees)
{
	if (spatialPose)
	{
		spatialPose->rotate.x = rx_degrees;
		spatialPose->rotate.y = ry_degrees;
		spatialPose->rotate.z = rz_degrees;
		return 1;
	}
	return -1;
}

// scale
inline a3i32 a3spatialPoseSetScale(a3_SpatialPose* spatialPose, const a3f32 sx, const a3f32 sy, const a3f32 sz)
{
	if (spatialPose)
	{
		spatialPose->scale.x = sx;
		spatialPose->scale.y = sy;
		spatialPose->scale.z = sz;
		return 1;
	}
	return -1;
}

// translation
inline a3i32 a3spatialPoseSetTranslation(a3_SpatialPose* spatialPose, const a3f32 tx, const a3f32 ty, const a3f32 tz)
{
	if (spatialPose)
	{
		spatialPose->translate.x = tx;
		spatialPose->translate.y = ty;
		spatialPose->translate.z = tz;
		return 1;
	}
	return -1;
}


//-----------------------------------------------------------------------------

// reset single node pose
inline a3i32 a3spatialPoseReset(a3_SpatialPose* spatialPose)
{
	if (spatialPose)
	{
		spatialPose->rotate = a3vec3_zero;
		spatialPose->scale = a3vec3_one;
		spatialPose->transform = a3mat4_identity;
		spatialPose->translate = a3vec3_zero;
		return 1;
	}
	return -1;
}

// convert single node pose to matrix
inline a3i32 a3spatialPoseConvert(a3mat4* mat_out, const a3_SpatialPose* spatialPose_in, const a3_SpatialPoseChannel channel, const a3_SpatialPoseEulerOrder order)
{
	if (mat_out && spatialPose_in)
	{
		switch (order) 
		{
			//M = Transform * ((rotation order) * Scale) 
			case a3poseEulerOrder_xyz: 
				mat_out->v3.xyz = spatialPose_in->translate;
				a3real4x4SetRotateXYZ(mat_out->m, 
					spatialPose_in->rotate.x, spatialPose_in->rotate.y, spatialPose_in->rotate.z);
				a3real3MulS(mat_out->m[0], spatialPose_in->scale.x);
				a3real3MulS(mat_out->m[1], spatialPose_in->scale.y);
				a3real3MulS(mat_out->m[2], spatialPose_in->scale.z);
				break;
			case a3poseEulerOrder_yzx: 
				mat_out->v3.xyz = spatialPose_in->translate;

				a3real4x4SetRotateY(mat_out->m, spatialPose_in->rotate.y);
				a3real4x4SetRotateZ(mat_out->m, spatialPose_in->rotate.z);
				a3real4x4SetRotateX(mat_out->m, spatialPose_in->rotate.x);

				a3real3MulS(mat_out->m[0], spatialPose_in->scale.x);
				a3real3MulS(mat_out->m[1], spatialPose_in->scale.y);
				a3real3MulS(mat_out->m[2], spatialPose_in->scale.z);
				break;
			case a3poseEulerOrder_zxy: 
				mat_out->v3.xyz = spatialPose_in->translate;

				a3real4x4SetRotateZ(mat_out->m, spatialPose_in->rotate.z);
				a3real4x4SetRotateX(mat_out->m, spatialPose_in->rotate.x);
				a3real4x4SetRotateY(mat_out->m, spatialPose_in->rotate.y);

				a3real3MulS(mat_out->m[0], spatialPose_in->scale.x);
				a3real3MulS(mat_out->m[1], spatialPose_in->scale.y);
				a3real3MulS(mat_out->m[2], spatialPose_in->scale.z);
				break;
			case a3poseEulerOrder_yxz:
				mat_out->v3.xyz = spatialPose_in->translate;

				a3real4x4SetRotateY(mat_out->m, spatialPose_in->rotate.y);
				a3real4x4SetRotateX(mat_out->m, spatialPose_in->rotate.x);
				a3real4x4SetRotateZ(mat_out->m, spatialPose_in->rotate.z);

				a3real3MulS(mat_out->m[0], spatialPose_in->scale.x);
				a3real3MulS(mat_out->m[1], spatialPose_in->scale.y);
				a3real3MulS(mat_out->m[2], spatialPose_in->scale.z);
				break;
			case a3poseEulerOrder_xzy: 
				mat_out->v3.xyz = spatialPose_in->translate;

				a3real4x4SetRotateX(mat_out->m, spatialPose_in->rotate.x);
				a3real4x4SetRotateZ(mat_out->m, spatialPose_in->rotate.z);
				a3real4x4SetRotateY(mat_out->m, spatialPose_in->rotate.y);

				a3real3MulS(mat_out->m[0], spatialPose_in->scale.x);
				a3real3MulS(mat_out->m[1], spatialPose_in->scale.y);
				a3real3MulS(mat_out->m[2], spatialPose_in->scale.z);
				break;
			case a3poseEulerOrder_zyx: 
				mat_out->v3.xyz = spatialPose_in->translate;

				a3real4x4SetRotateZ(mat_out->m, spatialPose_in->rotate.z);
				a3real4x4SetRotateY(mat_out->m, spatialPose_in->rotate.y);
				a3real4x4SetRotateX(mat_out->m, spatialPose_in->rotate.x);

				a3real3MulS(mat_out->m[0], spatialPose_in->scale.x);
				a3real3MulS(mat_out->m[1], spatialPose_in->scale.y);
				a3real3MulS(mat_out->m[2], spatialPose_in->scale.z);
				break;
		}
		return 1;
	}
	return -1;
}

// copy operation for single node pose
inline a3i32 a3spatialPoseCopy(a3_SpatialPose* spatialPose_out, const a3_SpatialPose* spatialPose_in)
{
	if (spatialPose_out && spatialPose_in)
	{
		spatialPose_out->rotate = spatialPose_in->rotate;
		spatialPose_out->scale = spatialPose_in->scale;
		spatialPose_out->transform = spatialPose_in->transform;
		spatialPose_out->translate = spatialPose_in->translate;
		return 1;
	}
	return -1;
}


inline a3i32 a3spatialPoseConcat(a3_SpatialPose* out_spatial, const a3_SpatialPose* sp_lh, const a3_SpatialPose* sp_rh) 
{
	if (sp_lh && sp_rh) 
	{
		a3real3Sum(out_spatial->rotate.v, sp_lh->rotate.v, sp_rh->rotate.v);
		out_spatial->scale = sp_lh->scale;
		a3real3MulComp(out_spatial->scale.v, sp_rh->scale.v);


		a3real3Sum(out_spatial->translate.v, sp_lh->translate.v, sp_rh->translate.v);
		return 1;
	}
	return -1;
}

inline a3i32 a3spatialPoseLerp(a3_SpatialPose* out_spatial, const a3_SpatialPose* sp_lh, const a3_SpatialPose* sp_rh, const a3f32 u)
{
	a3real3Lerp(out_spatial->rotate.v, sp_lh->rotate.v, sp_rh->rotate.v, u);

	//TO-DO: Swap this over to inverse lerp function
	a3real3Lerp(out_spatial->scale.v, sp_lh->scale.v, sp_rh->scale.v, u);

	a3real3Lerp(out_spatial->translate.v, sp_lh->translate.v, sp_rh->translate.v, u);
	return -1;
}



//-----------------------------------------------------------------------------


#endif	// !__ANIMAL3D_SPATIALPOSE_INL
#endif	// __ANIMAL3D_SPATIALPOSE_H
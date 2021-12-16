#include "..\a3_HierarchyStateBlend.h"
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
	
	a3_HierarchyStateBlend.inl
	Implementation of inline hierarchical blend operations.
*/


#ifdef __ANIMAL3D_HIERARCHYSTATEBLEND_H
#ifndef __ANIMAL3D_HIERARCHYSTATEBLEND_INL
#define __ANIMAL3D_HIERARCHYSTATEBLEND_INL


//-----------------------------------------------------------------------------

// pointer-based reset/identity operation for single spatial pose
inline a3_SpatialPose* a3spatialPoseOpIdentity(a3_SpatialPose* pose_out)
{
	a3spatialPoseReset(pose_out);

	// done
	return pose_out;
}

// pointer-based LERP operation for single spatial pose
inline a3_SpatialPose* a3spatialPoseOpLERP(a3_SpatialPose* pose_out, a3_SpatialPoseBlendArgs args)
{
	/* TO-DO: Make sure these are all the correct calculations*/
	if (args.pose0 && args.pose1)
	{
		pose_out->rotate = a3vec4Lerp(args.pose0->rotate, args.pose1->rotate, (a3real)args.param0);

		pose_out->scale = a3vec4LogLerp(args.pose0->scale, args.pose1->scale, (a3real)args.param0);

		pose_out->translate = a3vec4Lerp(args.pose0->translate, args.pose1->translate, (a3real)args.param0);


	}
	// done
	return pose_out;
}


/*Equivalent to a constructor, this operation returns/sets a pose constructed using the components provided.
Formats: constructr,s,t( ); poser,s,t( ).
Return: new pose with validated control values as components.
Controls (3): vectors representing rotation angles, scale and translation.*/
inline a3_SpatialPose* a3spatialPoseOpConstruct(a3_SpatialPose* pose_out, a3vec4 angles, a3vec4 scale, a3vec4 translation)
{
	pose_out->rotate = angles;
	pose_out->scale = scale;
	pose_out->translate = translation;
	return pose_out;
}

/*Equivalent to unary plus/positive (constant) or the assignment operator (copy), this operation simply returns/sets the unchanged control pose.  Note: These can be the same operation or broken into two, depending on the language (C/C++) and/or how you arrange the function parameters.
Formats: constantP( ); copyP( ); plusP( ).
Return: control pose.
Controls (1): spatial pose.
Kinda Confused on how this one works
*/
inline a3_SpatialPose* a3spatialPoseOpCopy(a3_SpatialPose* pose_out, a3_SpatialPoseBlendArgs args)
{
	a3spatialPoseCopy(pose_out, args.pose0);
	return pose_out;
}


/*Equivalent to unary minus/negative, this operation calculates the opposite/inverse pose description that "undoes" the control pose.  Note that this may not be a literal negation as each component may follow different rules for inversion.
Formats: negateP( ); invertP( ); minusP( ).
Return: inverted/negated control pose.
Controls (1): spatial pose.*/
inline a3_SpatialPose* a3spatialPoseOpInvert(a3_SpatialPose* pose_out, a3_SpatialPoseBlendArgs args)
{
	//I think this is right but I could be wrong
	pose_out->rotate = args.pose0->rotate;
	a3real4MulS(pose_out->rotate.v, -1);

	pose_out->scale = args.pose0->scale;
	a3real4MulS(pose_out->scale.v, -1);

	pose_out->transformMat = args.pose0->transformMat;
	a3real4x4MulS(pose_out->transformMat.m, -1);


	return pose_out;
}

inline a3_SpatialPose* a3spatialPoseOpConcat(a3_SpatialPose* pose_out, a3_SpatialPoseBlendArgs args)
{
	a3spatialPoseConcat(pose_out, args.pose0, args.pose1);
	return pose_out;
}

inline a3_SpatialPose* a3spatialPoseOpNearest(a3_SpatialPose* pose_out, a3_SpatialPoseBlendArgs args)
{

	if (args.param0 < 0.5f)
	{
		*pose_out = *args.pose0;
	}
	else if (args.param0 >= 0.5f)
	{
		*pose_out = *args.pose1;
	}

	return pose_out;
}

inline a3_SpatialPose* a3spatialPoseOpCubic(a3_SpatialPose* pose_out, a3_SpatialPoseBlendArgs args)
{
	a3real mu2 = (a3real)args.param0 * (a3real)args.param0;
	a3_SpatialPoseBlendArgs temp;

	a3_SpatialPose a0[1];

	temp.pose0 = args.pose3;
	temp.pose1 = args.pose2;
	a3spatialPoseOpDeconcat(a0, args);

	temp.pose0 = a0;
	temp.pose1 = args.pose0;
	a3spatialPoseOpDeconcat(a0, temp);

	temp.pose1 = args.pose1;
	a3spatialPoseOpConcat(a0, args);

	a3_SpatialPose a1[1];

	temp.pose0 = args.pose0;
	temp.pose1 = args.pose1;
	a3spatialPoseOpDeconcat(a1, temp);

	temp.pose0 = a0;
	temp.pose1 = a1;
	a3spatialPoseOpDeconcat(a1, temp);

	a3_SpatialPose a2[1];
	temp.pose0 = args.pose2;
	temp.pose1 = args.pose0;
	a3spatialPoseOpDeconcat(a2, temp);

	temp.pose0 = a0;
	temp.pose1 = a1;
	temp.pose2 = a2;
	temp.param0 = args.param0;
	temp.param1 = mu2;

	a3spatialPoseOpTriangular(pose_out, temp);

	return pose_out;
}

inline a3_SpatialPose* a3spatialPoseOpDeconcat(a3_SpatialPose* pose_out, a3_SpatialPoseBlendArgs args)
{

	a3_SpatialPoseBlendArgs tempArgs;
	tempArgs.pose0 = args.pose1;
	a3spatialPoseOpInvert(args.pose1, tempArgs);
	a3spatialPoseConcat(pose_out, args.pose0, args.pose1);

	return pose_out;

}

inline a3_SpatialPose* a3spatialPoseOpScale(a3_SpatialPose* pose_out, a3_SpatialPoseBlendArgs args)
{
	a3spatialPoseOpIdentity(pose_out);
	args.pose1 = pose_out;

	return a3spatialPoseOpLERP(pose_out, args);
}

inline a3_SpatialPose* a3spatialPoseOpTriangular(a3_SpatialPose* pose_out, a3_SpatialPoseBlendArgs args)
{
	a3real u0 = 1 - (a3real)args.param0 - (a3real)args.param1;

	a3_SpatialPose a0[1];
	a3_SpatialPose a1[1];
	a3_SpatialPose a2[1];
	a3_SpatialPoseBlendArgs temp;
	temp.pose0 = args.pose0;
	temp.param0 = u0;

	a3spatialPoseOpScale(a0, temp);
	temp.pose0 = args.pose1;
	temp.param0 = args.param0;
	a3spatialPoseOpScale(a1, temp);

	temp.pose0 = args.pose2;
	a3spatialPoseOpScale(a2, temp);

	a3spatialPoseConcat(pose_out, a0, a1);
	a3spatialPoseConcat(pose_out, pose_out, a2);

	return pose_out;
}

inline a3_SpatialPose* a3spatialPoseOpBiNearest(a3_SpatialPose* pose_out, a3_SpatialPoseBlendArgs args)
{
	a3_SpatialPose tmp0[1];
	a3_SpatialPose tmp1[1];

	a3_SpatialPoseBlendArgs temp;

	temp.pose0 = args.pose0;
	temp.pose1 = args.pose1;
	temp.param0 = args.param0;
	a3spatialPoseOpNearest(tmp0, temp);

	temp.pose0 = args.pose2;
	temp.pose1 = args.pose3;
	temp.param0 = args.param2;
	a3spatialPoseOpNearest(tmp1, temp);

	temp.pose0 = tmp0;
	temp.pose1 = tmp1;
	temp.param0 = args.param3;
	a3spatialPoseOpNearest(pose_out, temp);
	return pose_out;
}

inline a3_SpatialPose* a3spatialPoseOpBiLinear(a3_SpatialPose* pose_out, a3_SpatialPoseBlendArgs args)
{

	a3_SpatialPose tmp[1];
	a3_SpatialPoseBlendArgs temp;

	temp.pose0 = args.pose0;
	temp.pose1 = args.pose1;
	temp.param0 = args.param0;
	a3spatialPoseOpLERP(tmp, temp);

	temp.pose0 = args.pose2;
	temp.pose1 = args.pose3;
	temp.param0 = args.param1;
	a3spatialPoseOpLERP(pose_out, temp);

	temp.pose0 = tmp;
	temp.pose1 = pose_out;
	temp.param0 = args.param2;
	a3spatialPoseOpLERP(pose_out, temp);

	return pose_out;
}

inline a3_SpatialPose* a3spatialPoseOpSmoothStep(a3_SpatialPose* pose_out, a3_SpatialPoseBlendArgs args)
{
	//https://www.febucci.com/2018/08/easing-functions/

	a3real u = (a3real)args.param0;
	a3real easeIn = u * u;
	a3real easeOut = 1.0f - (((a3real)1.0f - u) * ((a3real)1.0f - u));

	u = a3lerp(easeIn, easeOut, u);
	a3spatialPoseLerp(pose_out, args.pose0, args.pose1, u);

	return pose_out;
}

inline a3_SpatialPose* a3spatialPoseOpDescale(a3_SpatialPose* pose_out, a3_SpatialPoseBlendArgs args)
{
	a3spatialPoseOpInvert(pose_out, args);
	args.pose0 = pose_out;
	a3spatialPoseOpScale(pose_out, args);
	return pose_out;
}

inline a3_SpatialPose* a3spatialPoseOpConvert(a3_SpatialPose* pose_out, a3_SpatialPoseBlendArgs args)
{

	a3spatialPoseConvert(args.pose0, a3poseChannel_none, a3poseEulerOrder_xyz);
	pose_out = args.pose0;

	return pose_out;
}

inline a3_SpatialPose* a3spatialPoseOpRevert(a3_SpatialPose* pose_out, a3_SpatialPoseBlendArgs args)
{
	pose_out = args.pose0;
	pose_out->transformMat = a3mat4_identity;
	return pose_out;
}

inline a3_SpatialPose* a3spatialPoseOpBiCubic(a3_SpatialPose* pose_out, a3_SpatialPose* poses0[4], a3_SpatialPose* poses1[4], a3_SpatialPose* poses2[4], a3_SpatialPose* poses3[4], a3real u[5])
{
	a3_SpatialPoseBlendArgs temp;
	a3_SpatialPose pose1[1];
	a3_SpatialPose pose2[1];
	a3_SpatialPose pose3[1];
	a3_SpatialPose pose4[1];

	temp.pose0 = poses0[0];
	temp.pose1 = poses0[1];
	temp.pose2 = poses0[2];
	temp.pose3 = poses0[3];
	temp.param0 = u[0];

	a3spatialPoseOpCubic(pose1, temp);

	temp.pose0 = poses1[0];
	temp.pose1 = poses1[1];
	temp.pose2 = poses1[2];
	temp.pose3 = poses1[3];
	temp.param0 = u[1];
	a3spatialPoseOpCubic(pose2, temp);

	temp.pose0 = poses2[0];
	temp.pose1 = poses2[1];
	temp.pose2 = poses2[2];
	temp.pose3 = poses2[3];
	temp.param0 = u[2];
	a3spatialPoseOpCubic(pose3, temp);

	temp.pose0 = poses3[0];
	temp.pose1 = poses3[1];
	temp.pose2 = poses3[2];
	temp.pose3 = poses3[3];
	temp.param0 = u[3];
	a3spatialPoseOpCubic(pose4, temp);

	temp.pose0 = pose1;
	temp.pose1 = pose2;
	temp.pose2 = pose3;
	temp.pose3 = pose4;
	temp.param0 = u[4];
	a3spatialPoseOpCubic(pose_out, temp);

	return pose_out;
}


//-----------------------------------------------------------------------------

// data-based reset/identity
/*
	a3mat4 transform;
	a3vec4 orientation;
	a3vec4 angles;
	a3vec4 scale;
	a3vec4 translation;
*/
inline a3_SpatialPose a3spatialPoseDOpIdentity()
{
	a3_SpatialPose const result = { a3mat4_identity, a3real_zero, a3real_zero, a3real_one, a3real_zero };
	return result;
}

// data-based LERP
inline a3_SpatialPose a3spatialPoseDOpLERP(a3_SpatialPose const pose0, a3_SpatialPose const pose1, a3real const u)
{
	a3_SpatialPose result = { 0 };
	//a3spatialPoseLERP(result, pose0, pose1, u);

	// done
	return result;
}


//-----------------------------------------------------------------------------

// pointer-based reset/identity operation for hierarchical pose
inline a3_HierarchyPose* a3hierarchyPoseOpIdentity(a3_HierarchyPose* pose_out, a3ui32 num_nodes)
{
	for (a3_SpatialPose* pose = pose_out->pose; pose < pose_out->pose + num_nodes; pose++) {
		a3spatialPoseOpIdentity(pose);
	}
	// done
	return pose_out;
}

// pointer-based LERP operation for hierarchical pose
inline a3_HierarchyPose* a3hierarchyPoseOpLERP(a3_HierarchyPose* pose_out, a3_HierarchyPose const* pose0, a3_HierarchyPose const* pose1, a3real const u, a3ui32 num_nodes)
{
	a3_SpatialPose* spose_out = pose_out->pose;
	a3_SpatialPoseBlendArgs temp;
	temp.pose0 = pose0->pose;
	temp.pose1 = pose1->pose;
	temp.param0 = u;

	for (a3ui32 i = 0; i < num_nodes; i++, spose_out++, temp.pose0++, temp.pose1++) {
		a3spatialPoseOpLERP(spose_out, temp);
	}
	// done
	a3spatialPoseLerp(pose_out->pose, pose0->pose, pose1->pose, u);

	return pose_out;
}


inline a3_HierarchyPose* a3hierarchyPoseOpConstruct(a3_HierarchyPose* pose_out, a3vec4 orientation, a3vec4 scale, a3vec4 translation, a3ui32 num_nodes)
{
	a3_SpatialPose* spose_out = pose_out->pose;


	for (a3ui32 i = 0; i < num_nodes; i++, spose_out++) {
		a3spatialPoseOpConstruct(spose_out, orientation, scale, translation);
	}
	return pose_out;
}


inline  a3_HierarchyPose* a3hierarchyPoseOpCopy(a3_HierarchyPose* pose_out, a3_HierarchyPose const* pose, a3ui32 num_nodes)
{
	a3_SpatialPose* spose_out = pose_out->pose;
	a3_SpatialPoseBlendArgs temp;
	temp.pose0 = pose->pose;

	for (a3ui32 i = 0; i < num_nodes; i++, spose_out++, temp.pose0++) {
		a3spatialPoseOpCopy(spose_out, temp);
	}
	return pose_out;
}

inline  a3_HierarchyPose* a3hierarchyPoseOpInvert(a3_HierarchyPose* pose_out, a3_HierarchyPose const* pose, a3ui32 num_nodes)
{
	a3_SpatialPose* spose_out = pose_out->pose;
	a3_SpatialPoseBlendArgs temp;
	temp.pose0 = pose->pose;

	for (a3ui32 i = 0; i < num_nodes; i++, spose_out++, temp.pose0++) {
		a3spatialPoseOpInvert(spose_out, temp);
	}
	return pose_out;
}

inline a3_HierarchyPose* a3hierarchyPoseOpConcat(a3_HierarchyPose* pose_out, a3_HierarchyPose const* pose0, a3_HierarchyPose const* pose1, a3ui32 num_nodes)
{
	a3_SpatialPose* spose_out = pose_out->pose;
	a3_SpatialPoseBlendArgs temp;
	temp.pose0 = pose0->pose;
	temp.pose1 = pose1->pose;

	for (a3ui32 i = 0; i < num_nodes; i++, spose_out++, temp.pose0++, temp.pose1++) {
		a3spatialPoseOpConcat(spose_out, temp);
	}
	return pose_out;
}

inline a3_HierarchyPose* a3hierarchyPoseOpNearest(a3_HierarchyPose* pose_out, a3_HierarchyPose const* pose0, a3_HierarchyPose const* pose1, a3real const u, a3ui32 num_nodes)
{
	a3_SpatialPose* spose_out = pose_out->pose;

	a3_SpatialPoseBlendArgs temp;
	temp.pose0 = pose0->pose;
	temp.pose1 = pose1->pose;
	temp.param0 = u;

	for (a3ui32 i = 0; i < num_nodes; i++, spose_out++, temp.pose0++, temp.pose1++) {
		a3spatialPoseOpNearest(spose_out, temp);
	}
	return pose_out;
}

inline a3_HierarchyPose* a3hierarchyPoseOpCubic(a3_HierarchyPose* pose_out, a3_HierarchyPose const* pose0, a3_HierarchyPose const* pose1, a3_HierarchyPose const* pose2, a3_HierarchyPose const* pose3, a3real const u, a3ui32 num_nodes)
{
	a3_SpatialPose* spose_out = pose_out->pose;
	a3_SpatialPoseBlendArgs temp;
	temp.param0 = u;

	for (a3ui32 i = 0; i < num_nodes; i++) {
		temp.pose0 = pose0->pose + i;
		temp.pose1 = pose1->pose + i;
		temp.pose2 = pose2->pose + i;
		temp.pose3 = pose3->pose + i;
		a3spatialPoseOpCubic(spose_out + i, temp);
	}
	return pose_out;
}

inline a3_HierarchyPose* a3hierarchyPoseOpDeconcat(a3_HierarchyPose* pose_out, a3_HierarchyPose const* pose0, a3_HierarchyPose const* pose1, a3ui32 num_nodes)
{
	a3_SpatialPose* spose_out = pose_out->pose;
	a3_SpatialPoseBlendArgs temp;
	temp.pose0 = pose0->pose;
	temp.pose1 = pose1->pose;

	for (a3ui32 i = 0; i < num_nodes; i++, spose_out++, temp.pose0++, temp.pose1++) {
		a3spatialPoseOpDeconcat(spose_out, temp);
	}
	return pose_out;
}

inline a3_HierarchyPose* a3hierarchyPoseOpScale(a3_HierarchyPose* pose_out, a3_HierarchyPose const* pose0, a3real const u, a3ui32 num_nodes)
{
	a3_SpatialPose* spose_out = pose_out->pose;
	a3_SpatialPoseBlendArgs temp;
	temp.pose0 = pose0->pose;
	temp.param0 = u;

	for (a3ui32 i = 0; i < num_nodes; i++, spose_out++, temp.pose0++) {
		a3spatialPoseOpScale(spose_out, temp);
	}
	return pose_out;
}

inline a3_HierarchyPose* a3hierarchyPoseOpTriangular(a3_HierarchyPose* pose_out, a3_HierarchyPose const* pose0, a3_HierarchyPose const* pose1, a3_HierarchyPose const* pose2, a3real const u1, a3real const u2, a3ui32 num_nodes)
{
	a3_SpatialPose* spose_out = pose_out->pose;
	a3_SpatialPoseBlendArgs temp;
	temp.pose0 = pose0->pose;
	temp.pose1 = pose1->pose;
	temp.pose2 = pose2->pose;
	temp.param0 = u1;
	temp.param1 = u2;
	for (a3ui32 i = 0; i < num_nodes; i++, spose_out++, temp.pose0++, temp.pose1++, temp.pose2++) {
		a3spatialPoseOpTriangular(spose_out, temp);
	}
	return pose_out;
}

inline a3_HierarchyPose* a3hierarchyPoseOpBiNearest(a3_HierarchyPose* pose_out, a3_HierarchyPose const* pose0, a3_HierarchyPose const* pose1, a3_HierarchyPose const* pose2, a3_HierarchyPose const* pose3, a3real const u1, a3real const u2, a3real const u3, a3ui32 num_nodes)
{
	a3_SpatialPose* spose_out = pose_out->pose;
	a3_SpatialPoseBlendArgs temp;
	temp.pose0 = pose0->pose;
	temp.pose1 = pose1->pose;
	temp.pose2 = pose2->pose;
	temp.pose3 = pose3->pose;
	temp.param0 = u1;
	temp.param1 = u2;
	temp.param2 = u3;
	for (a3ui32 i = 0; i < num_nodes; i++, spose_out++, temp.pose0++, temp.pose1++, temp.pose2++, temp.pose3++) {
		a3spatialPoseOpBiNearest(spose_out, temp);
	}
	return pose_out;
}

inline a3_HierarchyPose* a3hierarchyPoseOpBiLinear(a3_HierarchyPose* pose_out, a3_HierarchyPose const* pose0, a3_HierarchyPose const* pose1, a3_HierarchyPose const* pose2, a3_HierarchyPose const* pose3, a3real const u1, a3real const u2, a3real const u3, a3ui32 num_nodes)
{
	a3_SpatialPose* spose_out = pose_out->pose;
	a3_SpatialPoseBlendArgs temp;

	temp.pose0 = pose0->pose;
	temp.pose1 = pose1->pose;
	temp.pose2 = pose2->pose;
	temp.pose3 = pose3->pose;
	temp.param0 = u1;
	temp.param1 = u2;
	temp.param2 = u3;

	for (a3ui32 i = 0; i < num_nodes; i++, spose_out++, temp.pose0++, temp.pose1++, temp.pose2++, temp.pose3++) {
		a3spatialPoseOpBiLinear(spose_out, temp);
	}
	return pose_out;
}

inline a3_HierarchyPose* a3hierarchyPoseOpBiCubic(a3_HierarchyPose* pose_out, a3_HierarchyPose const* poses0[4], a3_HierarchyPose const* poses1[4], a3_HierarchyPose const* poses2[4], a3_HierarchyPose const* poses3[4], a3real const u[5], a3ui32 num_nodes)
{
	a3_SpatialPose* spose_out = pose_out->pose;
	a3_SpatialPose* spose0[4];
	spose0[0] = poses0[0]->pose;
	spose0[1] = poses0[1]->pose;
	spose0[2] = poses0[2]->pose;
	spose0[3] = poses0[3]->pose;

	a3_SpatialPose* spose1[4];
	spose1[0] = poses1[0]->pose;
	spose1[1] = poses1[1]->pose;
	spose1[2] = poses1[2]->pose;
	spose1[3] = poses1[3]->pose;

	a3_SpatialPose* spose2[4];
	spose2[0] = poses2[0]->pose;
	spose2[1] = poses2[1]->pose;
	spose2[2] = poses2[2]->pose;
	spose2[3] = poses2[3]->pose;

	a3_SpatialPose* spose3[4];
	spose3[0] = poses3[0]->pose;
	spose3[1] = poses3[1]->pose;
	spose3[2] = poses3[2]->pose;
	spose3[3] = poses3[3]->pose;

	for (a3ui32 i = 0; i < num_nodes; i++, spose_out++) {
		a3spatialPoseOpBiCubic(spose_out, spose0, spose1, spose2, spose3, u);
	}
	return pose_out;
}

inline a3_HierarchyPose* a3hierarchyPoseOpSmoothLERP(a3_HierarchyPose* pose_out, a3_HierarchyPose const* pose0, a3_HierarchyPose const* pose1, a3real const u, a3ui32 num_nodes)
{
	a3_SpatialPose* spose_out = pose_out->pose;
	a3_SpatialPoseBlendArgs temp;
	temp.pose0 = pose0->pose;
	temp.pose1 = pose1->pose;
	temp.param0 = u;

	for (a3ui32 i = 0; i < num_nodes; i++, spose_out++, temp.pose0++, temp.pose1++) {
		a3spatialPoseOpSmoothStep(spose_out, temp);
	}
	// done
	a3spatialPoseLerp(pose_out->pose, pose0->pose, pose1->pose, u);
	return pose_out;
}

inline a3_HierarchyPose* a3hierarchyPoseOpDeScale(a3_HierarchyPose* pose_out, a3_HierarchyPose const* pose0, a3real const u, a3ui32 num_nodes)
{
	a3_SpatialPose* spose_out = pose_out->pose;
	a3_SpatialPoseBlendArgs temp;
	temp.pose0 = pose0->pose;
	temp.param0 = u;

	for (a3ui32 i = 0; i < num_nodes; i++, spose_out++, temp.pose0++) {
		a3spatialPoseOpDescale(spose_out, temp);
	}
	return pose_out;
}

inline a3_HierarchyPose* a3hierarchyPoseOpConvert(a3_HierarchyPose* pose_out, a3_HierarchyPose const* pose, a3ui32 num_nodes)
{
	a3_SpatialPose* spose_out = pose_out->pose;
	a3_SpatialPoseBlendArgs temp;
	temp.pose0 = pose->pose;

	for (a3ui32 i = 0; i < num_nodes; i++, spose_out++, temp.pose0++) {
		a3spatialPoseOpConvert(pose_out->pose, temp);
	}
	return pose_out;

}

inline a3_HierarchyPose* a3hierarchyPoseOpRevert(a3_HierarchyPose* pose_out, a3_HierarchyPose const* pose, a3ui32 num_nodes)
{
	a3_SpatialPose* spose_out = pose_out->pose;
	a3_SpatialPoseBlendArgs temp;
	temp.pose0 = pose->pose;
	for (a3ui32 i = 0; i < num_nodes; i++, spose_out++, temp.pose0++) {
		a3spatialPoseOpRevert(pose_out->pose, temp);
	}

	return pose_out;
}

inline a3_HierarchyPose* a3hierarchyPoseOKinematics(a3_HierarchyPose* pose_out, a3_Hierarchy const* hierarchy, a3_HierarchyPose const* poseObj, a3_HierarchyPose const* poseLoc)
{
	const a3_HierarchyNode* itr = hierarchy->nodes;
	const a3_HierarchyNode* const end = itr + hierarchy->numNodes;
	for (; itr < end; ++itr)
	{
		if (itr->parentIndex >= 0)
			a3real4x4Product(poseObj->pose[itr->index].transformMat.m,
				poseObj->pose[itr->parentIndex].transformMat.m,
				poseLoc->pose[itr->index].transformMat.m);
		else
			poseObj->pose[itr->index] = poseLoc->pose[itr->index];
	}
	pose_out->pose = poseObj->pose;
	return pose_out;
}

inline a3_HierarchyPose* a3hierarchyPoseOpInverseKinematics(a3_HierarchyPose* pose_out, a3_Hierarchy const* hierarchy, a3_HierarchyPose const* poseObj, a3_HierarchyPose const* poseLoc)
{
	const a3_HierarchyNode* itr = hierarchy->nodes;
	const a3_HierarchyNode* const end = itr + hierarchy->numNodes;
	a3hierarchyPoseOpInvert(pose_out, poseObj, hierarchy->numNodes);

	for (; itr < end; ++itr)
	{
		if (itr->parentIndex >= 0)
			a3real4x4Product(poseLoc->pose[itr->index].transformMat.m,
				pose_out->pose[itr->parentIndex].transformMat.m,
				poseObj->pose[itr->index].transformMat.m);
		else
			poseLoc->pose[itr->index] = poseObj->pose[itr->index];
	}
	pose_out->pose = poseLoc->pose;

	return pose_out;
}

inline a3_SpatialPose* a3ExecuteBlendNode(a3_SpatialPose* spatialPose_Out, a3_SpatialPoseBlendNodeType* node)
{
	node->BlendType(spatialPose_Out, node->values);
	return spatialPose_Out;
}


//-----------------------------------------------------------------------------


#endif	// !__ANIMAL3D_HIERARCHYSTATEBLEND_INL
#endif	// __ANIMAL3D_HIERARCHYSTATEBLEND_H
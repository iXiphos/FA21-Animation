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
	pose_out->transform = a3mat4_identity;
	pose_out->orientation = (a3vec4){ 0, 0, 0, 1};
	pose_out->scale = a3vec4_one;
	pose_out->translation = a3vec4_zero;
	pose_out->angles = a3vec4_zero;

	// done
	return pose_out;
}

// pointer-based LERP operation for single spatial pose
inline a3_SpatialPose* a3spatialPoseOpLERP(a3_SpatialPose* pose_out, a3_SpatialPose const* pose0, a3_SpatialPose const* pose1, a3real const u)
{
	/* TO-DO: Make sure these are all the correct calculations*/
	if (pose0 && pose1)
	{
		a3real3NLerp(pose_out->angles.v, pose0->angles.v, pose1->angles.v, u);

		/**** TO-DO: Update this to be exponential lerp instead of regular lerp   ****/
		a3real3Lerp(pose_out->scale.v, pose0->scale.v, pose1->scale.v, u);

		a3real3Lerp(pose_out->translation.v, pose0->translation.v, pose1->translation.v, u);

		a3real3Lerp(pose_out->transform.v, pose0->transform.v, pose1->transform.v, u);

		return 1;
	}
	// done
	return pose_out;
}


/*Equivalent to a constructor, this operation returns/sets a pose constructed using the components provided.
Formats: constructr,s,t( ); poser,s,t( ).
Return: new pose with validated control values as components.
Controls (3): vectors representing rotation angles, scale and translation.*/
inline a3_SpatialPose* a3spatialPoseOpConstruct(a3_SpatialPose* pose_out, a3vec4 orientation, a3vec4 scale, a3vec4 translation)
{
	pose_out->orientation = orientation;
	pose_out->scale = scale;
	pose_out->translation = translation;
	return pose_out;
}

/*Equivalent to unary plus/positive (constant) or the assignment operator (copy), this operation simply returns/sets the unchanged control pose.  Note: These can be the same operation or broken into two, depending on the language (C/C++) and/or how you arrange the function parameters.
Formats: constantP( ); copyP( ); plusP( ).
Return: control pose.
Controls (1): spatial pose.
Kinda Confused on how this one works
*/
inline a3_SpatialPose* a3spatialPoseOpCopy(a3_SpatialPose* pose_out, a3_SpatialPose const* pose)
{
	a3spatialPoseCopy(pose_out, pose);
	return pose_out;
}


/*Equivalent to unary minus/negative, this operation calculates the opposite/inverse pose description that "undoes" the control pose.  Note that this may not be a literal negation as each component may follow different rules for inversion.
Formats: negateP( ); invertP( ); minusP( ).
Return: inverted/negated control pose.
Controls (1): spatial pose.*/
inline a3_SpatialPose* a3spatialPoseOpInvert(a3_SpatialPose* pose_out, a3_SpatialPose const* pose)
{
	//I think this is right but I could be wrong
	a3real4MulS(pose->angles.v, -1);
	pose_out->angles = pose->angles;

	a3real4MulS(pose->scale.v, -1);
	pose_out->scale = pose->scale;

	a3real4x4MulS(pose->transform.v, -1);
	pose_out->transform = pose->transform;

	return pose_out;
}

inline a3_SpatialPose* a3spatialPoseOpConcat(a3_SpatialPose* pose_out, a3_SpatialPose const* pose0, a3_SpatialPose const* pose1)
{
	a3spatialPoseConcat(pose_out, pose0, pose1);
	return pose_out;
}

inline a3_SpatialPose* a3spatialPoseOpNearest(a3_SpatialPose* pose_out, a3_SpatialPose const* pose0, a3_SpatialPose const* pose1, a3real const u)
{

	if (u < 0.5f) 
	{
		pose_out = pose0;
	}
	else if (u >= 0.5f) 
	{
		pose_out = pose1;
	}

	return pose_out;
}

inline a3_SpatialPose* a3spatialPoseOpCubic(a3_SpatialPose* pose_out, a3_SpatialPose const* pose0, a3_SpatialPose const* pose1, a3_SpatialPose const* pose2, a3_SpatialPose const* pose3, a3real const u)
{
	a3real mu2 = u * u;

	//This is the formula, unsure if I have to do each part individually or if there is a better wa, I know for addition I can concat
	//But unsure about subtraction
	a3_SpatialPose* a0 = y3 - y2 - y0 + y1;
	a3_SpatialPose* a1 = y0 - y1 - a0;
	a3_SpatialPose* a2 = y2 - y0;
	a3_SpatialPose* a3 = y1;

	pose_out = (a0 * u * mu2 + a1 * mu2 + a2 * u + a3);


	return pose_out;
}

inline a3_SpatialPose* a3spatialPoseOpDeconcat(a3_SpatialPose* pose_out, a3_SpatialPose const* pose0, a3_SpatialPose const* pose1)
{

	pose_out->angles.x = a3trigValid_sind(pose0->angles.x - pose1->angles.x);
	pose_out->angles.y = a3trigValid_sind(pose0->angles.y - pose1->angles.y);
	pose_out->angles.z = a3trigValid_sind(pose0->angles.z - pose1->angles.z);

	pose_out->scale.x = pose0->scale.x / pose1->scale.x;
	pose_out->scale.y = pose0->scale.y / pose1->scale.y;
	pose_out->scale.z = pose0->scale.z / pose1->scale.z;

	pose_out->translation.x = pose0->translation.x - pose1->translation.x;
	pose_out->translation.y = pose0->translation.y - pose1->translation.y;
	pose_out->translation.z = pose0->translation.z - pose1->translation.z;

	return pose_out;

}

inline a3_SpatialPose* a3spatialPoseOpScale(a3_SpatialPose* pose_out, a3_SpatialPose const* pose0, a3real const u)
{


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
	a3_SpatialPose const result = { a3mat4_identity, a3vec4_zero, a3vec4_zero, a3vec4_one, a3vec4_zero};
	return result;
}

// data-based LERP
inline a3_SpatialPose a3spatialPoseDOpLERP(a3_SpatialPose const pose0, a3_SpatialPose const pose1, a3real const u)
{
	a3_SpatialPose result = { 0 };
	a3spatialPoseOpLERP(&result, &pose0, &pose1, u);

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
inline a3_HierarchyPose* a3hierarchyPoseOpLERP(a3_HierarchyPose* pose_out, a3_HierarchyPose const* pose0, a3_HierarchyPose const* pose1, a3ui32 num_nodes, a3real const u)
{
	a3_SpatialPose* spose_out = pose_out->pose;
	a3_SpatialPose* spose0 = pose0->pose;
	a3_SpatialPose* spose1 = pose1->pose;

	for (a3ui32 i = 0; i < num_nodes; i++, spose_out++, spose0++, spose1++) {
		a3spatialPoseOpLERP(spose_out, spose0, spose1, u);
	}
	// done
	a3spatialPoseLerp(pose_out->pose, pose0->pose, pose1->pose, u);

	return pose_out;
}


inline a3_HierarchyPose* a3hierarchyOpConstruct(a3_HierarchyPose* pose_out, a3vec4 orientation, a3vec4 scale, a3vec4 translation, a3ui32 num_nodes)
{
	a3_SpatialPose* spose_out = pose_out->pose;

	for (a3ui32 i = 0; i < num_nodes; i++, spose_out++) {
		a3spatialPoseOponstruct(spose_out, orientation, scale, translation);
	}
	return pose_out;
}


inline  a3_HierarchyPose* a3hierarchyOpCopy(a3_HierarchyPose* pose_out, a3_HierarchyPose const* pose, a3ui32 num_nodes)
{
	a3_SpatialPose* spose_out = pose_out->pose;
	a3_SpatialPose* spose = pose->pose;

	for (a3ui32 i = 0; i < num_nodes; i++, spose_out++, spose++) {
		a3spatialPoseOpCopy(spose_out, spose);
	}
	return pose_out;
}

inline  a3_HierarchyPose* a3hierarchyOpInvert(a3_HierarchyPose* pose_out, a3_HierarchyPose const* pose, a3ui32 num_nodes)
{
	a3_SpatialPose* spose_out = pose_out->pose;
	a3_SpatialPose* spose = pose->pose;

	for (a3ui32 i = 0; i < num_nodes; i++, spose_out++, spose++) {
		a3spatialPoseOpInvert(spose_out, spose);
	}
	return pose_out;
}

inline a3_HierarchyPose* a3hierarchyOpConcat(a3_HierarchyPose* pose_out, a3_HierarchyPose const* pose0, a3_HierarchyPose const* pose1, a3ui32 num_nodes)
{
	a3_SpatialPose* spose_out = pose_out->pose;
	a3_SpatialPose* spose0 = pose0->pose;
	a3_SpatialPose* spose1 = pose1->pose;

	for (a3ui32 i = 0; i < num_nodes; i++, spose_out++, spose0++, spose1++) {
		a3spatialPoseOpConcat(spose_out, spose0, spose1);
	}
	return pose_out;
}

inline a3_HierarchyPose* a3hierarchyOpNearest(a3_HierarchyPose* pose_out, a3_HierarchyPose const* pose0, a3_HierarchyPose const* pose1, a3real const u, a3ui32 num_nodes)
{
	a3_SpatialPose* spose_out = pose_out->pose;
	a3_SpatialPose* spose0 = pose0->pose;
	a3_SpatialPose* spose1 = pose1->pose;

	for (a3ui32 i = 0; i < num_nodes; i++, spose_out++, spose0++, spose1++) {
		a3spatialPoseOpNearest(spose_out, spose0, spose1, u);
	}
	return pose_out;
}

inline a3_HierarchyPose* a3hierarchyOpCubic(a3_HierarchyPose* pose_out, a3_HierarchyPose const* pose0, a3_HierarchyPose const* pose1, a3_HierarchyPose const* pose2, a3_HierarchyPose const* pose3, a3real const u, a3ui32 num_nodes)
{
	a3_SpatialPose* spose_out = pose_out->pose;
	a3_SpatialPose* spose0 = pose0->pose;
	a3_SpatialPose* spose1 = pose1->pose;
	a3_SpatialPose* spose2 = pose2->pose;
	a3_SpatialPose* spose3 = pose3->pose;

	for (a3ui32 i = 0; i < num_nodes; i++) {
		a3spatialPoseOpCubic(spose_out + i, spose0 + i, spose1 + i, spose2 + i, spose3 + i, u);
	}
	return pose_out;
}




//-----------------------------------------------------------------------------


#endif	// !__ANIMAL3D_HIERARCHYSTATEBLEND_INL
#endif	// __ANIMAL3D_HIERARCHYSTATEBLEND_H
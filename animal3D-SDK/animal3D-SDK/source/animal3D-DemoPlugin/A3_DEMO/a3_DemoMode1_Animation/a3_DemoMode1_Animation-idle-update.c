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
	
	a3_DemoMode1_Animation-idle-update.c
	Demo mode implementations: animation scene.

	********************************************
	*** UPDATE FOR ANIMATION SCENE MODE      ***
	********************************************
*/

//-----------------------------------------------------------------------------

#include "../a3_DemoMode1_Animation.h"

//typedef struct a3_DemoState a3_DemoState;
#include "../a3_DemoState.h"

#include "../_a3_demo_utilities/a3_DemoMacros.h"


//-----------------------------------------------------------------------------
// UPDATE

void a3demo_update_objects(a3_DemoState* demoState, a3f64 const dt,
	a3_DemoSceneObject* sceneObjectBase, a3ui32 count, a3boolean useZYX, a3boolean applyScale);
void a3demo_update_defaultAnimation(a3_DemoState* demoState, a3f64 const dt,
	a3_DemoSceneObject* sceneObjectBase, a3ui32 count, a3ui32 axis);
void a3demo_update_bindSkybox(a3_DemoSceneObject* obj_camera, a3_DemoSceneObject* obj_skybox);
void a3demo_update_pointLight(a3_DemoSceneObject* obj_camera, a3_DemoPointLight* pointLightBase, a3ui32 count);

void a3demo_applyScale_internal(a3_DemoSceneObject* sceneObject, a3real4x4p s);

a3i32 a3animate_updateSkeletonRenderMats(a3_Hierarchy const* hierarchy,
	a3mat4* renderArray, a3mat4* renderAxesArray, a3mat4 const* objectSpaceArray, a3mat4 const mvp_obj);

void a3animation_update(a3_DemoState* demoState, a3_DemoMode1_Animation* demoMode, a3f64 const dt)
{
	a3ui32 i;
	a3_DemoModelMatrixStack matrixStack[animationMaxCount_sceneObject];

	// active camera
	a3_DemoProjector const* activeCamera = demoMode->projector + demoMode->activeCamera;
	a3_DemoSceneObject const* activeCameraObject = activeCamera->sceneObject;

	// temp scale mat
	a3mat4 scaleMat = a3mat4_identity;

	a3demo_update_objects(demoState, dt,
		demoMode->object_scene, animationMaxCount_sceneObject, 0, 0);
	a3demo_update_objects(demoState, dt,
		demoMode->object_camera, animationMaxCount_cameraObject, 1, 0);

	a3demo_updateProjectorViewProjectionMat(demoMode->proj_camera_main);

	// apply scales to objects
	for (i = 0; i < animationMaxCount_sceneObject; ++i)
		a3demo_applyScale_internal(demoMode->object_scene + i, scaleMat.m);

	// update skybox
	a3demo_update_bindSkybox(demoMode->obj_camera_main, demoMode->obj_skybox);

	// update matrix stack data
	for (i = 0; i < animationMaxCount_sceneObject; ++i)
	{
		a3demo_updateModelMatrixStack(matrixStack + i,
			activeCamera->projectionMat.m, activeCameraObject->modelMat.m, activeCameraObject->modelMatInv.m,
			demoMode->object_scene[i].modelMat.m, a3mat4_identity.m);
	}


	a3hierarchyPoseCopy(&demoMode->hierarchyState_skel->localSpacePose, demoMode->hierarchyPoseGroup_skel->posePool, demoMode->hierarchy_skel->numNodes);
	a3hierarchyPoseConvert(&demoMode->hierarchyState_skel->localSpacePose, demoMode->hierarchy_skel->numNodes, 0, a3poseEulerOrder_xyz);
	a3kinematicsSolveForward(demoMode->hierarchyState_skel);

	a3mat4 mvp_skeleton;
	a3real4x4Product(mvp_skeleton.m, activeCamera->viewProjectionMat.m, demoMode->obj_skeleton->modelMat.m);

	// update graphics
	a3_SpatialPose* poses = demoMode->hierarchyState_skel->objectSpacePose.spatialPose;
	for (a3ui32 i = 0; i < demoMode->hierarchy_skel->numNodes; i++) {
		a3mat4* bone = demoMode->skeletonPose_transformLMVP_bone + i;
		a3mat4* joint = demoMode->skeletonPose_transformLMVP_joint + i;
		*bone = poses[i].transform;



		// for joint axes rendering
		a3real4x4SetScale(joint->m, 0.25f);
		a3real4x4Concat(bone->m, joint->m);
		a3real4x4Concat(mvp_skeleton.m, joint->m);
	}



	/*
	a3animate_updateSkeletonRenderMats(demoMode->hierarchy_skel,
		demoMode->skeletonPose_render, demoMode->skeletonPose_renderAxes, demoMode->skeletonPose_object,
		mvp_skeleton);
*/

	a3bufferRefillOffset(demoState->ubo_transformLMVP_bone, 0, 0, sizeof(demoMode->skeletonPose_transformLMVP_bone), demoMode->skeletonPose_transformLMVP_bone);
	a3bufferRefillOffset(demoState->ubo_transformLMVP_joint, 0, 0, sizeof(demoMode->skeletonPose_transformLMVP_joint), demoMode->skeletonPose_transformLMVP_joint);

}
void animation_updateSkeletonLocalSpace(a3_Hierarchy const* hierarchy,
	a3mat4* localSpaceArray,
	a3_SpatialPose const keyPoseArray[animateMaxCount_skeletonPose][animateMaxCount_skeletonJoint]) {

	if (hierarchy && localSpaceArray && keyPoseArray)
	{
		for (a3ui32 i = 0; i < hierarchy->numNodes; i++) {

			//a3spatialPoseConvert();

		}
	}
}

void animation_updateSkeletonObjectSpace(a3_Hierarchy const* hierarchy,
	a3mat4* const objectSpaceArray, a3mat4 const* const localSpaceArray) {

}
/*
a3i32 a3animate_updateSkeletonRenderMats(a3_Hierarchy const* hierarchy,
	a3mat4* renderArray, a3mat4* renderAxesArray, a3mat4 const* objectSpaceArray, a3mat4 const mvp_obj)
{
	if (hierarchy && renderArray && objectSpaceArray)
	{
		a3real3rk up;
		a3ui32 j;
		a3i32 jp;
		for (j = 0;
			j < hierarchy->numNodes;
			++j)
		{
			jp = hierarchy->nodes[j].parentIndex;
			if (jp >= 0)
			{
				renderArray[j] = a3mat4_identity;
				a3real3Diff(renderArray[j].m[2], objectSpaceArray[j].m[3], objectSpaceArray[jp].m[3]);
				up = (renderArray[j].m20 * renderArray[j].m21) ? a3vec3_z.v : a3vec3_y.v;
				a3real3MulS(a3real3CrossUnit(renderArray[j].m[0], up, renderArray[j].m[2]), 0.25f);
				a3real3MulS(a3real3CrossUnit(renderArray[j].m[1], renderArray[j].m[2], renderArray[j].m[0]), 0.25f);
				renderArray[j].v3 = objectSpaceArray[jp].v3;
			}
			else
			{
				// zero scale at root position
				a3real4x4SetScale(renderArray[j].m, 0.0f);
				renderArray[j].v3 = objectSpaceArray[j].v3;
			}
			a3real4x4Concat(mvp_obj.m, renderArray[j].m);

			// for joint axes rendering
			a3real4x4SetScale(renderAxesArray[j].m, 0.25f);
			a3real4x4Concat(objectSpaceArray[j].m, renderAxesArray[j].m);
			a3real4x4Concat(mvp_obj.m, renderAxesArray[j].m);
		}

		// done
		return 1;
	}
	return -1;
}
*/
//-----------------------------------------------------------------------------

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

#include "../_animation/a3_NodeEditor.h"

//-----------------------------------------------------------------------------
// UTILS

inline a3real4r a3demo_mat2quat_safe(a3real4 q, a3real4x4 const m)
{
	// ****TO-DO: 
	//	-> convert rotation part of matrix to quaternion
	//	-> NOTE: this is for testing dual quaternion skinning only; 
	//		quaternion data would normally be computed with poses

	a3real4SetReal4(q, a3vec4_w.v);

	// done
	return q;
}

inline a3real4x2r a3demo_mat2dquat_safe(a3real4x2 Q, a3real4x4 const m)
{
	// ****TO-DO: 
	//	-> convert matrix to dual quaternion
	//	-> NOTE: this is for testing dual quaternion skinning only; 
	//		quaternion data would normally be computed with poses

	a3demo_mat2quat_safe(Q[0], m);
	a3real4SetReal4(Q[1], a3vec4_zero.v);

	// done
	return Q;
}


//-----------------------------------------------------------------------------



//-----------------------------------------------------------------------------
// UPDATE

void a3demo_update_objects(a3f64 const dt, a3_DemoSceneObject* sceneObjectBase,
	a3ui32 count, a3boolean useZYX, a3boolean applyScale);
void a3demo_update_defaultAnimation(a3_DemoState* demoState, a3f64 const dt,
	a3_DemoSceneObject* sceneObjectBase, a3ui32 count, a3ui32 axis);
void a3demo_update_bindSkybox(a3_DemoSceneObject* obj_camera, a3_DemoSceneObject* obj_skybox);
void a3demo_update_pointLight(a3_DemoSceneObject* obj_camera, a3_DemoPointLight* pointLightBase, a3ui32 count);

void a3demo_applyScale_internal(a3_DemoSceneObject* sceneObject, a3real4x4p s);

void a3animation_updateUI(a3_DemoState* demoState, a3_DemoMode1_Animation* demoMode, a3f64 const dt);

void a3animation_update_graphics(a3_DemoState* demoState, a3_DemoMode1_Animation* demoMode,
	a3_DemoModelMatrixStack const* matrixStack, a3_HierarchyState const* activeHS)
{
	// active camera
	a3_DemoProjector const* activeCamera = demoMode->projector + demoMode->activeCamera;
	a3_DemoSceneObject const* activeCameraObject = activeCamera->sceneObject;

	// temp scale mat
	a3mat4 scaleMat = a3mat4_identity;
	a3addressdiff const skeletonIndex = demoMode->obj_skeleton - demoMode->object_scene;
	a3ui32 const mvp_size = demoMode->hierarchy_skel->numNodes * sizeof(a3mat4);
	a3ui32 const t_skin_size = sizeof(demoMode->t_skin);
	a3ui32 const dq_skin_size = sizeof(demoMode->dq_skin);
	a3mat4 const mvp_obj = matrixStack[skeletonIndex].modelViewProjectionMat;
	a3mat4* mvp_joint, * mvp_bone, * t_skin;
	a3dualquat* dq_skin;
	a3index i;
	a3i32 p;

	// update joint and bone transforms
	for (i = 0; i < demoMode->hierarchy_skel->numNodes; ++i)
	{
		mvp_joint = demoMode->mvp_joint + i;
		mvp_bone = demoMode->mvp_bone + i;
		t_skin = demoMode->t_skin + i;
		dq_skin = demoMode->dq_skin + i;

		// joint transform
		a3real4x4SetScale(scaleMat.m, a3real_sixth);
		a3real4x4Concat(activeHS->objectSpace->pose[i].transformMat.m, scaleMat.m);
		a3real4x4Product(mvp_joint->m, mvp_obj.m, scaleMat.m);

		// bone transform
		p = demoMode->hierarchy_skel->nodes[i].parentIndex;
		if (p >= 0)
		{
			// position is parent joint's position
			scaleMat.v3 = activeHS->objectSpace->pose[p].transformMat.v3;

			// direction basis is from parent to current
			a3real3Diff(scaleMat.v2.v,
				activeHS->objectSpace->pose[i].transformMat.v3.v, scaleMat.v3.v);

			// right basis is cross of some upward vector and direction
			// select 'z' for up if either of the other dimensions is set
			a3real3MulS(a3real3CrossUnit(scaleMat.v0.v,
				a3real2LengthSquared(scaleMat.v2.v) > a3real_zero
				? a3vec3_z.v : a3vec3_y.v, scaleMat.v2.v), a3real_sixth);

			// up basis is cross of direction and right
			a3real3MulS(a3real3CrossUnit(scaleMat.v1.v,
				scaleMat.v2.v, scaleMat.v0.v), a3real_sixth);
		}
		else
		{
			// if we are a root joint, make bone invisible
			a3real4x4SetScale(scaleMat.m, a3real_zero);
		}
		a3real4x4Product(mvp_bone->m, mvp_obj.m, scaleMat.m);

		// get base to current object-space
		*t_skin = activeHS->objectSpaceBindToCurrent->pose[i].transformMat;

		// calculate DQ
		a3demo_mat2dquat_safe(dq_skin->Q, t_skin->m);
	}

	// upload
	a3bufferRefill(demoState->ubo_transformMVP, 0, mvp_size, demoMode->mvp_joint);
	a3bufferRefill(demoState->ubo_transformMVPB, 0, mvp_size, demoMode->mvp_bone);
	a3bufferRefill(demoState->ubo_transformBlend, 0, t_skin_size, demoMode->t_skin);
	a3bufferRefillOffset(demoState->ubo_transformBlend, 0, t_skin_size, dq_skin_size, demoMode->dq_skin);
}

void a3animation_update_fk(a3_HierarchyState* activeHS,
	a3_HierarchyState const* baseHS, a3_HierarchyPoseGroup const* poseGroup)
{
	if (activeHS->hierarchy == baseHS->hierarchy &&
		activeHS->hierarchy == poseGroup->hierarchy)
	{
		// FK pipeline
		a3hierarchyPoseConcat(activeHS->localSpace,	// local: goal to calculate
			activeHS->animPose, // holds current sample pose
			baseHS->localSpace, // holds base pose (animPose is all identity poses)
			activeHS->hierarchy->numNodes);
		// cancle out root motion in animation
		activeHS->localSpace->pose->translate = baseHS->localSpace->pose->translate;

		a3hierarchyPoseConvert(activeHS->localSpace,
			activeHS->hierarchy->numNodes,
			poseGroup->channel,
			poseGroup->order);
		a3kinematicsSolveForward(activeHS);
	}
}

void a3animation_update_ik(a3_HierarchyState* activeHS,
	a3_HierarchyState const* baseHS, a3_HierarchyPoseGroup const* poseGroup)
{
	if (activeHS->hierarchy == baseHS->hierarchy &&
		activeHS->hierarchy == poseGroup->hierarchy)
	{
		// IK pipeline
		// ****TO-DO: direct opposite of FK
		a3hierarchyPoseDeconcat(activeHS->localSpace,	// local: goal to calculate
			activeHS->animPose, // holds current sample pose
			baseHS->localSpace, // holds base pose (animPose is all identity poses)
			activeHS->hierarchy->numNodes);
		a3hierarchyPoseRestore(activeHS->localSpace,
			activeHS->hierarchy->numNodes,
			poseGroup->channel,
			poseGroup->order);
		a3kinematicsSolveInverse(activeHS);
	}
}

void a3animation_update_skin(a3_HierarchyState* activeHS,
	a3_HierarchyState const* baseHS)
{
	if (activeHS->hierarchy == baseHS->hierarchy)
	{
		// FK pipeline extended for skinning and other applications
		a3hierarchyStateUpdateLocalInverse(activeHS);
		a3hierarchyStateUpdateObjectInverse(activeHS);
		a3hierarchyStateUpdateObjectBindToCurrent(activeHS, baseHS);
	}
}

void a3animation_update_nodehierarchy(a3_DemoMode1_Animation* demoMode, a3f64 const dt);

void a3animation_update_applyEffectors(a3_DemoMode1_Animation* demoMode,
	a3_HierarchyState* activeHS, a3_HierarchyState const* baseHS, a3_HierarchyPoseGroup const* poseGroup)
{

}

void a3animation_update_animation(a3_DemoMode1_Animation* demoMode, a3f64 const dt,
	a3boolean const updateIK)
{
	a3_ClipController* clipCtrl_1 = demoMode->clipCtrlA;
	a3_ClipController* clipCtrl_2 = demoMode->clipCtrlB;
	a3ui32 sampleIndex0, sampleIndex1;

	a3_HierarchyState* activeHS = demoMode->hierarchyState_skel_final;
	a3_HierarchyState const* baseHS = demoMode->hierarchyState_skel_base;
	a3_HierarchyPoseGroup const* poseGroup = demoMode->hierarchyPoseGroup_skel;

	a3_HierarchyState* newState = demoMode->hierarchyState_skel_final;

	newState->animPose->pose = activeHS->animPose->pose; //Set this equal to the final Pose


	a3clipControllerUpdate(clipCtrl_1, dt);
	sampleIndex0 = demoMode->clipPool->keyframe[clipCtrl_1->keyframeIndex].sampleIndex0;
	sampleIndex1 = demoMode->clipPool->keyframe[clipCtrl_1->keyframeIndex].sampleIndex1;



	a3hierarchyPoseCopy(activeHS->animPose,
		newState->animPose,
		activeHS->hierarchy->numNodes);

	// run FK pipeline (skinning optional)
	//a3animation_update_fk(activeHS, baseHS, poseGroup);
	a3animation_update_skin(activeHS, baseHS);


	for (a3i32 i = 0; i < demoMode->nodeClipCount; i++) {
		a3clipControllerUpdate(demoMode->nodeClipCtrls + i, dt);
	}
	


}

void a3animation_add_nodehierarchy(a3_DemoMode1_Animation* demoMode, a3i32 index, a3i32 parent_node_index, a3byte parent_pin_index) {
	NodeEditorNode* node = demoMode->nodeEditorCtx->nodes + index;
	a3i32 p_index;
	a3_SpatialPoseBlendNode* dst_array;
	switch (node->type.subtype) {
	case NodeEditorNodeSubtype_Param:
		p_index = demoMode->paramNodeCount++;
		dst_array = demoMode->paramNodes;
		break;


	case NodeEditorNodeSubtype_Clip:
		p_index = demoMode->clipNodeCount++;
		dst_array = demoMode->clipNodes;
		break;

	case NodeEditorNodeSubtype_Blend:
		p_index = demoMode->blendNodeCount++;
		dst_array = demoMode->blendNodes;
		break;
	default:
		return;
	}

	
	dst_array[p_index] = (a3_SpatialPoseBlendNode){
		.type_index=node->type.index,
		.output_node=parent_node_index,
		.output_pin=parent_pin_index
	};
	//a3_SpatialPoseBlendNodeType blendNodeType = demoMode->blendNodesType[node->type.index];

	for (a3i32 i = 0; i < demoMode->nodeEditorCtx->pins_count; i++) {
		NodeEditorPin* pin = demoMode->nodeEditorCtx->pins + i;
		if (pin->node_index == index)
			a3animation_add_nodehierarchy(demoMode, pin->input_index, p_index, pin->pin_index);
	}

	


}

void a3animation_update_nodehierarchy(a3_DemoMode1_Animation* demoMode, a3f64 const dt) {
	

	float param_options[10];

	param_options[0] = 0; //TODO: time
	param_options[1] = (a3real)demoMode->axis_l[0];
	param_options[2] = (a3real)demoMode->axis_l[1];
	param_options[3] = 0;
	param_options[4] = 0;
	param_options[5] = 0;
	param_options[6] = 0;
	param_options[7] = 0;
	param_options[8] = 0;
	param_options[9] = 0;


	// find root
	a3i32 root_index = 0;


	if (root_index == -1) return;


	if (demoMode->blendNodes)
		free(demoMode->blendNodes);
	// TODO: allocate values
	// TODO: only rebuild list if we need to
	NodeEditorCtx* ctx = demoMode->nodeEditorCtx;

	const size_t alloc_count = ctx->blend_node_count + ctx->param_node_count + ctx->clip_node_count;
	demoMode->blendNodes = (a3_SpatialPoseBlendNode*)malloc(alloc_count * sizeof(a3_SpatialPoseBlendNode));
	demoMode->paramNodes = demoMode->blendNodes + ctx->blend_node_count;
	demoMode->clipNodes = demoMode->paramNodes + ctx->param_node_count;

	demoMode->paramNodeCount = 0;
	demoMode->clipNodeCount = 0;
	demoMode->blendNodeCount = 0;



	// create hierarchy
	a3animation_add_nodehierarchy(demoMode, root_index, -1, -1);

	a3_SpatialPoseBlendArgs* args = (a3_SpatialPoseBlendArgs*)malloc(demoMode->blendNodeCount * sizeof(a3_SpatialPoseBlendArgs));
	// TODO: load base pose into 
	/*
	* (STEP 0): update clip controllers
	STEP 1: make array of actual blend nodes 
	STEP 2: copy poses from clip ctrl and floats from params
	STEP 3: from the bottom of the list, 
	STEP 3.1: do blend op
	STEP 3.2: copy result to correct destination
	GOTO 3.1

	STEP 4: apply to skeleton
	*/

	for (a3i32 i = 0; i < demoMode->paramNodeCount; i++) {
		a3_SpatialPoseBlendNode node = demoMode->paramNodes[i];
		// 
		args[node.output_node].params[node.output_pin] = param_options[node.type_index];
	}


	for (a3i32 i = 0; i < demoMode->clipNodeCount; i++) {
		a3_SpatialPoseBlendNode node = demoMode->clipNodes[i];
		//
		args[node.output_node].poses[node.output_pin];
	}

	a3_SpatialPose finalpose;

	// iterate through list of poses backwards, executing node and copying to destination
	for (a3i32 i = demoMode->blendNodeCount - 1; i >= 0; i--) {
		a3_SpatialPoseBlendNode node = demoMode->blendNodes[i];
		a3_SpatialPoseBlendNodeType type = demoMode->blendNodesType[node.type_index];
		a3_SpatialPose pose;
		type.function(&pose, args[i]);

		if (i != 0)
			args[node.output_node].poses[node.output_pin] = pose;
		else
			finalpose = pose;
	}


	// apply to skeleton

	free(args);
}

void a3animation_update_sceneGraph(a3_DemoMode1_Animation* demoMode, a3f64 const dt)
{
	a3ui32 i;
	a3mat4 scaleMat = a3mat4_identity;

	a3demo_update_objects(dt, demoMode->object_scene, animationMaxCount_sceneObject, 0, 0);
	a3demo_update_objects(dt, demoMode->obj_camera_main, 1, 1, 0);

	a3demo_updateProjectorViewProjectionMat(demoMode->proj_camera_main);

	// apply scales to objects
	for (i = 0; i < animationMaxCount_sceneObject; ++i)
	{
		a3demo_applyScale_internal(demoMode->object_scene + i, scaleMat.m);
	}

	// update skybox
	a3demo_update_bindSkybox(demoMode->obj_camera_main, demoMode->obj_skybox);

	for (i = 0; i < animationMaxCount_sceneObject; ++i)
		demoMode->sceneGraphState->localSpace->pose[i].transformMat = demoMode->object_scene[i].modelMat;
	a3kinematicsSolveForward(demoMode->sceneGraphState);
	a3hierarchyStateUpdateLocalInverse(demoMode->sceneGraphState);
	a3hierarchyStateUpdateObjectInverse(demoMode->sceneGraphState);
}

void a3animation_update(a3_DemoState* demoState, a3_DemoMode1_Animation* demoMode, a3f64 const dt)
{
	demoMode->axis_l[0] = (a3real)a3keyboardGetDifference(demoState->keyboard, a3key_D, a3key_A);
	demoMode->axis_l[1] = (a3real)a3keyboardGetDifference(demoState->keyboard, a3key_S, a3key_W);

	a3animation_updateUI(demoState, demoMode, dt);

	a3ui32 i;
	a3_DemoModelMatrixStack matrixStack[animationMaxCount_sceneObject];

	// active camera
	a3_DemoProjector const* activeCamera = demoMode->projector + demoMode->activeCamera;
	a3_DemoSceneObject const* activeCameraObject = activeCamera->sceneObject;

	// skeletal
	if (demoState->updateAnimation)
		a3animation_update_animation(demoMode, dt, 1);

	// update scene graph local transforms
	a3animation_update_sceneGraph(demoMode, dt);

	// update matrix stack data using scene graph
	for (i = 0; i < animationMaxCount_sceneObject; ++i)
	{
		a3demo_updateModelMatrixStack(matrixStack + i,
			activeCamera->projectionMat.m,
			demoMode->sceneGraphState->objectSpace->pose[demoMode->obj_camera_main->sceneGraphIndex].transformMat.m,
			demoMode->sceneGraphState->objectSpaceInv->pose[demoMode->obj_camera_main->sceneGraphIndex].transformMat.m,
			demoMode->sceneGraphState->objectSpace->pose[demoMode->object_scene[i].sceneGraphIndex].transformMat.m,
			a3mat4_identity.m);
	}

	// prepare and upload graphics data
	a3animation_update_graphics(demoState, demoMode, matrixStack, demoMode->hierarchyState_skel_final);

	// testing: reset IK effectors to lock them to FK result
	{
		//void a3animation_load_resetEffectors(a3_DemoMode1_Animation * demoMode,
		//	a3_HierarchyState * hierarchyState, a3_HierarchyPoseGroup const* poseGroup);
		//a3animation_load_resetEffectors(demoMode,
		//	demoMode->hierarchyState_skel_final, demoMode->hierarchyPoseGroup_skel);
	}

	
	//a3animation_update_nodehierarchy(demoMode, dt);

	// ****TO-DO:
	// process input
	//switch (demoMode->ctrl_position)
	//{
	//case animation_input_direct:
	//	demoMode->pos.x = (a3f32)(demoMode->axis_l[0] * dt);
	//	demoMode->pos.y = (a3f32)(demoMode->axis_l[1] * dt);
	//	break;

	//case animation_input_euler:
	//	demoMode->pos.x = demoMode->obj_skeleton_ctrl->position.x + (a3f32)(demoMode->axis_l[0] * dt);
	//	demoMode->pos.y = demoMode->obj_skeleton_ctrl->position.x + (a3f32)(demoMode->axis_l[1] * dt);
	//	break;

	//case animation_input_interpolate1:
	//	demoMode->pos.x = (a3f32)(demoMode->obj_skeleton_ctrl->position.x + (5 * demoMode->axis_l[0] - demoMode->obj_skeleton_ctrl->position.x) * dt);
	//	demoMode->pos.y = (a3f32)(demoMode->obj_skeleton_ctrl->position.y + (5 * demoMode->axis_l[1] - demoMode->obj_skeleton_ctrl->position.y) * dt);
	//	break;

	//case animation_input_interpolate2:
	//	demoMode->vel.x = (a3f32)(demoMode->vel.x + (5 * demoMode->axis_l[0] - demoMode->vel.x) * dt);
	//	demoMode->vel.y = (a3f32)(demoMode->vel.y + (5 * demoMode->axis_l[1] - demoMode->vel.y) * dt);

	//	demoMode->pos.x = demoMode->obj_skeleton_ctrl->position.x + (a3f32)(demoMode->vel.x * dt);
	//	demoMode->pos.y = demoMode->obj_skeleton_ctrl->position.y + (a3f32)(demoMode->vel.y * dt);
	//	break;

	//case animation_input_kinematic:
	//	demoMode->vel.x = (a3real)(demoMode->vel.x + demoMode->axis_l[0] * dt);
	//	demoMode->vel.y = (a3real)(demoMode->vel.y + demoMode->axis_l[1] * dt);

	//	demoMode->pos.x = demoMode->obj_skeleton_ctrl->position.x + demoMode->vel.x + (a3f32)(demoMode->axis_l[0] * (dt * dt) / 2.0f);
	//	demoMode->pos.y = demoMode->obj_skeleton_ctrl->position.y + demoMode->vel.y + (a3f32)(demoMode->axis_l[1] * (dt * dt) / 2.0f);
	//	break;
	//}

	//switch (demoMode->ctrl_rotation) {
	//case animation_input_direct:
	//	demoMode->rot = (a3f32)demoState->xcontrol->ctrl.rThumbX_unit * 180.0f;
	//	break;
	//case animation_input_euler:
	//	demoMode->velr = (a3f32)demoState->xcontrol->ctrl.rThumbX_unit * 180.0f;
	//	demoMode->rot = demoMode->obj_skeleton_ctrl->euler.z + (a3f32)(demoMode->velr * dt);
	//	break;
	//case animation_input_interpolate1:
	//	demoMode->rot = (a3f32)(demoMode->obj_skeleton_ctrl->euler.z + (demoState->xcontrol->ctrl.rThumbX_unit * 180 - demoMode->obj_skeleton_ctrl->euler.z) * dt);
	//	break;
	//case animation_input_interpolate2:
	//	//This needs to be velocity interpolate
	//	break;
	//case animation_input_kinematic:
	//	demoMode->velr = (a3real)(demoMode->velr + demoState->xcontrol->ctrl.rThumbX_unit * 180 * dt);
	//	demoMode->rot = demoMode->obj_skeleton_ctrl->euler.z + demoMode->velr + (a3f32)(demoState->xcontrol->ctrl.rThumbX_unit * 180 * (dt * dt) / 2.0f);
	//	break;
	//}

	// apply input
	demoMode->obj_skeleton_ctrl->position.x = +(demoMode->pos.x);
	demoMode->obj_skeleton_ctrl->position.y = +(demoMode->pos.y);
	demoMode->obj_skeleton_ctrl->euler.z = -a3trigValid_sind(demoMode->rot);
}

void a3animation_drawHierarchyUI(a3_Hierarchy* hierarchy, const char* title);
void a3animation_updateUI(a3_DemoState* demoState, a3_DemoMode1_Animation* demoMode, a3f64 const dt) {

	igShowDemoWindow(NULL);
	/*
	const char* format = "%.3f";


	igDragFloat3("look at", demoMode->obj_skeleton_neckLookat_ctrl->position.v, 0.5, -300, 300, format, 0);
	igDragFloat3("skeleton base", demoMode->obj_skeleton_ctrl->position.v, 0.5, -300, 300, format, 0);
	igDragFloat3("wrist effector", demoMode->obj_skeleton_wristEffector_r_ctrl->position.v, 0.5, -300, 300, format, 0);
	igDragFloat3("wrist constraint", demoMode->obj_skeleton_wristConstraint_r_ctrl->position.v, 0.5, -300, 300, format, 0);


	//igDrag("axix", demoMode->axis_l, 0.5, -10, 10, format, 0);
	igDragFloat2("vel", demoMode->vel.v, 0.5, -10, 10, format, 0);
	igDragFloat2("pos", demoMode->pos.v, 0.5, -10, 10, format, 0);


	// draw hierarchy
	a3animation_drawHierarchyUI(demoMode->hierarchy_skel, "Skeleton");
	*/
	
	a3_NodeEditor_DrawNodeList(demoMode);
	igSameLine(0, 0);
	a3_NodeEditorUpdate(demoMode->nodeEditorCtx);
	
}

ImRect a3animation_drawHierarchyNodeUI(a3_Hierarchy* hierarchy, a3ui32 index) {
	igPushItemWidth(400);
	bool open = igTreeNodeEx_Str(hierarchy->nodes[index].name, ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_SpanFullWidth);
	ImRect nodeRect;
	igGetItemRectMin(&nodeRect.Min);
	igGetItemRectMax(&nodeRect.Max);

	if (open) {
		const ImU32 TreeLineColor = igGetColorU32_Col(ImGuiCol_Text, 1.0f);
		const float SmallOffsetX = 11.0f; //for now, a hardcoded value; should take into account tree indent size
		ImDrawList* drawList = igGetWindowDrawList();

		ImVec2 verticalLineStart;
		igGetCursorScreenPos(&verticalLineStart);
		verticalLineStart.x += SmallOffsetX; //to nicely line up with the arrow symbol
		ImVec2 verticalLineEnd = verticalLineStart;


		for (a3ui32 i = index; i < hierarchy->numNodes; i++) {
			a3_HierarchyNode* node = hierarchy->nodes + i;
			if (node->parentIndex == index) {
				
				const ImRect childRect = a3animation_drawHierarchyNodeUI(hierarchy, i);
				const float midpoint = (childRect.Min.y + childRect.Max.y) / 2.0f;
				verticalLineEnd.y = midpoint;
			}
		}



		ImDrawList_AddLine(drawList, verticalLineStart, verticalLineEnd, TreeLineColor, 1.0f);
		igTreePop();
	}
	igPopItemWidth();

	

	return nodeRect;
}

void a3animation_drawHierarchyUI(a3_Hierarchy* hierarchy, const char* title) {
	if (igTreeNode_Str(title)) {
		a3animation_drawHierarchyNodeUI(hierarchy, 0);
		igTreePop();
	}
}

//-----------------------------------------------------------------------------

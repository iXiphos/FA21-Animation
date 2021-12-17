#include "../a3_NodeEditor.h"
#include "../../a3_DemoMode1_Animation.h"

const char* ParamNames[10] = {
	"time",
	"x_input",
	"y_input",
	"sin_time",
	"cos_time"
};

// rebuild our list of pins and links
// MUST BE CALLED BEFORE DOING ANYTHING WITH THESE LISTS IF ctx->isDirty
void a3_NodeEditorRebuildLists(NodeEditorCtx* ctx) {
	free(ctx->pins);
	ctx->pins = (NodeEditorPin*)malloc(ctx->pins_count * sizeof(NodeEditorPin));

	a3i32 pin_index = 0;
	for (a3i32 n = 0; n < ctx->nodes_count; n++) {
		const NodeEditorNode node = ctx->nodes[n];
		const NodeEditorNodeType type = node.type;
		a3i32 subpin_index = 0;
		for (a3i32 p = 0; p < type.ctrl_count; p++) {
			a3_NodeEditorPin_Set(ctx, pin_index++, n, subpin_index++, NodeEditorPinType_InCtrl);
		}

		for (a3i32 p = 0; p < type.param_count; p++) {
			a3_NodeEditorPin_Set(ctx, pin_index++, n, subpin_index++, NodeEditorPinType_InParam);
		}
		
		if (type.output_type != NodeEditorPinType_None)
			a3_NodeEditorPin_Set(ctx, pin_index++, n, 0, type.output_type);
	}

	ctx->isDirty = false;
	//free(ctx->links);
}

void a3_NodeEditorPin_Set(NodeEditorCtx* ctx, a3i32 index, a3i32 node_index, a3i32 pin_index, NodeEditorPinType type) {
	ctx->pins[index] =
		(NodeEditorPin){
			.index = index,
			.node_index = node_index,
			.type = type,
			.pin_index = pin_index,
			.input_index = -1
	};
	a3_NodeEditorPin_RefreshPos(ctx, index);
}

void a3_NodeEditorAddNode(NodeEditorCtx* ctx, NodeEditorNodeType type) {
	
	switch (type.subtype) {
	case NodeEditorNodeSubtype_Param:
		ctx->param_node_count++;
		break;
	case NodeEditorNodeSubtype_Blend:
		ctx->blend_node_count++;
		break;
	case NodeEditorNodeSubtype_Clip:
		ctx->clip_node_count++;
		break;
	}

	ctx->isDirty = true;
	ctx->pins_count += type.ctrl_count + type.param_count;
	if (type.output_type != NodeEditorPinType_None)
		ctx->pins_count++;

	const a3ui32 old_size = ctx->nodes_count * sizeof(NodeEditorNode);
	const a3ui32 new_size = old_size + sizeof(NodeEditorNode);

	NodeEditorNode* new_nodes = (NodeEditorNode*)malloc(new_size);
	if (ctx->nodes != NULL) {
		memcpy(new_nodes, ctx->nodes, old_size);
		free(ctx->nodes);
	}
	ctx->nodes = new_nodes;

	ctx->nodes[ctx->nodes_count] =
		(NodeEditorNode){
			.index = ctx->nodes_count,
			.type = type
	};

	ctx->nodes = new_nodes;
	ctx->nodes_count++;
}


void a3_NodeEditor_DrawNodeList(a3_DemoMode1_Animation* demoMode) {
	NodeEditorCtx* ctx = demoMode->nodeEditorCtx;
	a3ui32 const rate = 24;
	a3f64 const fps = (a3f64)rate;
	igBeginGroup();
	igBeginChild_Str(
		"nodes_region",
		(ImVec2) { 200.0f, 0.0f },
		true,
			ImGuiWindowFlags_NoMove);
	igPushItemWidth(200);
	if (igTreeNodeEx_Str("Nodes", ImGuiTreeNodeFlags_DefaultOpen)) {
		if (igTreeNodeEx_Str("Params", ImGuiTreeNodeFlags_DefaultOpen)) {
			for (a3i32 i = 0; i < 5; i++) {
				if (igButton(ParamNames[i], (ImVec2) { 0, 0 })) {
					NodeEditorNodeType type = (NodeEditorNodeType){
						.ctrl_count = 0,
						.param_count = 0,
						.output_type = NodeEditorPinType_OutParam,
						.subtype = NodeEditorNodeSubtype_Param,
						.index = i
					};
					strncpy(type.name, ParamNames[i], 25);
					type.name[24] = 0;
					type.name_len = (a3byte)strlen(type.name);
					a3_NodeEditorAddNode(ctx, type);
				}
			}
			igTreePop();
		}
		if (igTreeNodeEx_Str("Clips", ImGuiTreeNodeFlags_DefaultOpen )) {
			for (a3ui32 i = 0; i < demoMode->clipPool->clipCount; i++) {
				a3_Clip* clip = demoMode->clipPool->clip + i;
				if (igButton(clip->name, (ImVec2) { 0, 0 })) {
					NodeEditorNodeType type = (NodeEditorNodeType){
						.ctrl_count = 0,
						.param_count = 0,
						.output_type = NodeEditorPinType_OutCtrl,
						.subtype = NodeEditorNodeSubtype_Clip,
						.index = demoMode->nodeClipCount
					};
					strncpy(type.name, clip->name, 25);
					type.name[24] = 0;
					type.name_len = (a3byte)strlen(type.name);
					a3_NodeEditorAddNode(ctx, type);

					a3clipControllerSetClip(demoMode->nodeClipCtrls + demoMode->nodeClipCount, demoMode->clipPool, clip->index, rate, fps);
					demoMode->nodeClipCount++;
				}
			}
			igTreePop();
		}
		if (igTreeNodeEx_Str("Blend", ImGuiTreeNodeFlags_DefaultOpen)) {
			for (a3ui32 i = 0; i < 6; i++) {
				a3_SpatialPoseBlendNodeType blendnodetype = demoMode->blendNodesType[i];
				if (igButton(blendnodetype.name, (ImVec2) { 0, 0 })) {

					NodeEditorNodeType type = (NodeEditorNodeType){
						.ctrl_count = blendnodetype.ctrlCount,
						.param_count = blendnodetype.paramCount,
						.output_type = NodeEditorPinType_OutCtrl,
						.subtype = NodeEditorNodeSubtype_Blend,
						.index = i
					};
					strncpy(type.name, blendnodetype.name, 25);
					type.name[24] = 0;
					type.name_len = (a3byte)strlen(type.name);
					a3_NodeEditorAddNode(ctx, type);
				}
			}
			igTreePop();
		}
	

		igTreePop();
	}
	igPopItemWidth();
	igEndChild();
	igEndGroup();
}

void a3_NodeEditorUpdate(NodeEditorCtx* ctx) {

	



	igBeginGroup();
	igPushStyleVar_Vec2(ImGuiStyleVar_FramePadding, (ImVec2) { 1.f, 1.f });
	igPushStyleVar_Vec2(ImGuiStyleVar_WindowPadding, (ImVec2) {0.f, 0.f});
	igPushStyleColor_U32(ImGuiCol_ChildBg, IM_COL32(40, 40, 50, 200));
	igBeginChild_Str(
		"scrolling_region",
		(ImVec2) {
		0.0f, 0.0f
	},
		true,
			ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoMove |
			ImGuiWindowFlags_NoScrollWithMouse);


	
	// Input stuff
	
	ImVec2 mouse_pos;
	igGetMousePos(&mouse_pos);
	ctx->mouse_delta = (ImVec2){ mouse_pos.x - ctx->mouse_pos.x, mouse_pos.y - ctx->mouse_pos.y };
	ctx->mouse_pos = mouse_pos;
	ctx->mouse_clicked = igIsMouseClicked(0, false);
	ctx->mouse_released = igIsMouseReleased(0);
	ctx->mouse_held = !ctx->mouse_clicked && igIsMouseDown(0);
	

	ImVec2 origin;
	igGetCursorScreenPos(&origin);
	ctx->canvas_origin = (a3vec2){ origin.x, origin.y };
	igGetWindowSize(&ctx->canvas_size);

	ctx->CanvasDrawList = igGetWindowDrawList();
	ImVec2 canvas_size;
	igGetWindowSize(&canvas_size);
	// Draw Grid

	const ImVec2 offset = (ImVec2){ 0, 0 };
	float grid_spacing = 10.0f;
	const ImU32 LineColor = IM_COL32(200, 200, 200, 40);;
	//const ImVec2 origin = ctx->CanvasOriginScreenSpace;
	for (float x = fmodf(offset.x, grid_spacing); x < canvas_size.x;
		x += grid_spacing)
	{
		
		ImDrawList_AddLine(
			ctx->CanvasDrawList,
			(ImVec2) { origin.x + x, origin.y + 0.0f},
			(ImVec2) { origin.x + x, origin.y + canvas_size.y},
			LineColor, 1.0);
			
	}

	for (float y = fmodf(offset.y, grid_spacing); y < canvas_size.y;
		y += grid_spacing)
	{
		
		ImDrawList_AddLine(
			ctx->CanvasDrawList,
			(ImVec2) { origin.x + 0, origin.y + y},
			(ImVec2) { origin.x + canvas_size.x, origin.y + y},
			LineColor, 1.0);
			
	}


	if (ctx->isDirty) {
		a3_NodeEditorRebuildLists(ctx);
	}




	ctx->node_hovered = -1;
	ctx->pin_hovered = -1;


	for (a3i32 n = 0; n < ctx->nodes_count; n++) {
		a3_NodeEditorNode_Update(ctx, n);
	}
	for (a3i32 n = 0; n < ctx->pins_count; n++) {
		a3_NodeEditorPin_Update(ctx, n);
	}



	if (ctx->mouse_clicked) {

		ctx->pin_dragging = ctx->pin_hovered;

		if (ctx->pin_hovered == -1)
			ctx->node_selected = ctx->node_dragging = ctx->node_hovered;
		else
			ctx->node_dragging = -1;
	}

	if (ctx->mouse_released) {
		if (ctx->pin_dragging != -1 && ctx->pin_hovered != -1) {
			// check node types
			NodeEditorPin* start = ctx->pins + ctx->pin_dragging;
			NodeEditorPin* end = ctx->pins + ctx->pin_hovered;

			// started from an input
			if (start->type & NodeEditorPinType_MaskIn) {
				NodeEditorPin* tmp = start;
				start = end;
				end = tmp;
			}

			if ((start->type ^ NodeEditorPinType_MaskIn) == end->type)
				end->input_index = start->index;

		}


		ctx->node_dragging = -1;
		ctx->pin_dragging = -1;
	}

	for (a3i32 n = 0; n < ctx->nodes_count; n++) {
		a3_NodeEditorNode_Draw(ctx, n);
	}
	for (a3i32 n = 0; n < ctx->pins_count; n++) {
		a3_NodeEditorPin_Draw(ctx, n);
	}

	// END

	ImDrawList_ChannelsMerge(ctx->CanvasDrawList);

	igEndChild();
	igPopStyleColor(1); // pop child window background color
	igPopStyleVar(1);   // pop window padding
	igPopStyleVar(1);   // pop frame padding
	igEndGroup();

}


void a3_NodeEditorPin_RefreshPos(NodeEditorCtx* ctx, a3i32 index) {
	NodeEditorPin* pin = ctx->pins + index;
	const NodeEditorNode node = ctx->nodes[pin->node_index];
	float right = pin->type & NodeEditorPinType_MaskIn ? 10.0f : 90.0f;
	pin->pos = (ImVec2){ node.pos.x + right, node.pos.y + 30.0f + (15.0f * (float)pin->pin_index) };
}

void a3_NodeEditorPin_Update(NodeEditorCtx* ctx, a3i32 index) {
	NodeEditorPin* pin = ctx->pins + index;

	a3_NodeEditorPin_RefreshPos(ctx, index);

	ImVec2 center = (ImVec2){ pin->pos.x + ctx->canvas_origin.x, pin->pos.y + ctx->canvas_origin.y };

	ImVec2 mouse = ctx->mouse_pos;
	ImVec2 diff = (ImVec2){ (center.x - mouse.x), (center.y - mouse.y) };

	const float radius = 6.0f;
	bool mouse_over = (diff.x * diff.x + diff.y * diff.y) <= (radius * radius);

	if (mouse_over) {
		ctx->pin_hovered = index;
	}
}


void a3_NodeEditorPin_Draw(NodeEditorCtx* ctx, a3i32 index) {
	NodeEditorPin* pin = ctx->pins + index;
	const ImU32 pin_base_color = IM_COL32(255, 255, 255, 255);
	const ImU32 pin_ctrl_color = IM_COL32(255, 100, 100, 255);
	const ImU32 pin_param_color = IM_COL32(100, 100, 255, 255);
	const ImU32 white = IM_COL32(255, 255, 255, 255);

	ImU32 pin_color = pin->type & NodeEditorPinType_MaskCtrl ? pin_ctrl_color : pin_param_color;

	ImVec2 center = (ImVec2){ pin->pos.x + ctx->canvas_origin.x, pin->pos.y + ctx->canvas_origin.y };

	bool show_hover = ctx->pin_hovered == index && (ctx->pin_dragging == -1 || ((pin->type ^ NodeEditorPinType_MaskIn) == ctx->pins[ctx->pin_dragging].type));
	if (show_hover)
		ImDrawList_AddCircle(ctx->CanvasDrawList, center, 5.0f, pin_color, 10, 2.0f);
	else
		ImDrawList_AddCircleFilled(ctx->CanvasDrawList, center, 5.0f, pin_color, 10);

	
	if (ctx->pin_dragging == index) {
		ImDrawList_AddLine(ctx->CanvasDrawList, center, ctx->mouse_pos, pin_color, 2.0f);
	}
	else if (pin->input_index != -1) {
		ImVec2 dst = ctx->pins[pin->input_index].pos;
		dst.x += ctx->canvas_origin.x;
		dst.y += ctx->canvas_origin.y;
		ImDrawList_AddLine(ctx->CanvasDrawList, center, dst, pin_color, 2.0f);
	}
}


void a3_NodeEditorNode_Update(NodeEditorCtx* ctx, a3i32 index) {
	NodeEditorNode* node = ctx->nodes + index;


	if (ctx->mouse_held && ctx->node_dragging == index) {
		float maxX = ctx->canvas_size.x - 100.0f;
		float maxY = ctx->canvas_size.y - 100.0f;
		node->pos.x = a3clamp(0.0f, maxX, node->pos.x + ctx->mouse_delta.x);
		node->pos.y = a3clamp(0.0f, maxY, node->pos.y + ctx->mouse_delta.y);
	}

	a3vec2 offset = node->pos;
	a3real2Add(offset.v, ctx->canvas_origin.v);

	ImVec2 rect_min = (ImVec2) { offset.x, offset.y };
	ImVec2 rect_max = (ImVec2) { offset.x + 100.0f, offset.y + 100.0f };
	
	ImVec2 mouse = ctx->mouse_pos;
	bool mouse_over = mouse.x > rect_min.x && mouse.x < rect_max.x&& mouse.y > rect_min.y && mouse.y < rect_max.y;

	if (mouse_over) {
		ctx->node_hovered = index;
	}
	else if (ctx->node_dragging == index) {
		// we dragged off of the node
		ctx->node_dragging = -1;
	}

}


void a3_NodeEditorNode_Draw(NodeEditorCtx* ctx, a3i32 index) {
	NodeEditorNode* node = ctx->nodes + index;
	const ImU32 title_color = IM_COL32(255, 255, 255, 255);
	const ImU32 nodebase_color = IM_COL32(50, 50, 50, 255);
	const ImU32 nodehover_color = IM_COL32(75, 75, 75, 255);
	const ImU32 titlebar_param_color = IM_COL32(41, 74, 122, 255);
	const ImU32 titlebar_blend_color = IM_COL32(122, 74, 41, 255);
	const ImU32 titlebar_clip_color = IM_COL32(41, 122, 74, 255);
	const ImU32 titlebar_output_color = IM_COL32(255, 122, 74, 255);
	const ImU32 outline_color = IM_COL32(200, 200, 200, 255);
	const ImU32 node_color = index != ctx->node_hovered ? nodebase_color : nodehover_color;

	ImU32 titlebar_color;
	switch (node->type.subtype) {
	case NodeEditorNodeSubtype_Param:
		titlebar_color = titlebar_param_color;
		break;
	case NodeEditorNodeSubtype_Blend:
		titlebar_color = titlebar_blend_color;
		break;
	case NodeEditorNodeSubtype_Clip:
		titlebar_color = titlebar_clip_color;
		break;
	case NodeEditorNodeSubtype_Output:
		titlebar_color = titlebar_output_color;
		break;
	}

	a3vec2 offset = node->pos;
	a3real2Add(offset.v, ctx->canvas_origin.v);

	ImVec2 rect_min = (ImVec2){ offset.x, offset.y };
	ImVec2 rect_max = (ImVec2){ offset.x + 100.0f, offset.y + 100.0f };
	ImVec2 title_max = (ImVec2){ rect_max.x, offset.y + 20.0f };

	ImDrawList_AddRectFilled(ctx->CanvasDrawList, rect_min, rect_max, node_color, 10.0f, ImDrawFlags_RoundCornersAll);
	ImDrawList_AddRectFilled(ctx->CanvasDrawList, rect_min, title_max, titlebar_color, 10.0f, ImDrawFlags_RoundCornersTop);

	if (ctx->node_selected == index)
		ImDrawList_AddRect(ctx->CanvasDrawList, rect_min, rect_max, outline_color, 10.0f, ImDrawFlags_RoundCornersAll, 2.0f);

	ImVec2 title_start = (ImVec2){ offset.x + 10.0f, offset.y + 5.0f };
	ImDrawList_AddText_Vec2(ctx->CanvasDrawList, title_start, title_color, node->type.name, node->type.name + node->type.name_len);
}

void a3_NodeEditor_Process(a3_DemoMode1_Animation* demoMode) {

}
#include "../a3_NodeEditor.h"




// rebuild our list of pins and links
// MUST BE CALLED BEFORE DOING ANYTHING WITH THESE LISTS IF ctx->isDirty
void a3_NodeEditorRebuildLists(NodeEditorCtx* ctx) {
	free(ctx->pins);
	ctx->pins = (NodeEditorPin*)malloc(ctx->pins_count * sizeof(NodeEditorPin));

	a3i32 pin_index = 0;
	for (a3i32 n = 0; n < ctx->nodes_count; n++) {
		const NodeEditorNode node = ctx->nodes[n];
		for (a3i32 p = 0; p < node.pin_count; p++) {
			
			ImVec2 pos = (ImVec2){ node.pos.x + 10.0f, node.pos.y + 30.0f + ((float)p * 20.0f)};
			ctx->pins[pin_index] =
				(NodeEditorPin){
					.index = pin_index,
					.node_index = n,
					.pos = pos,
					.type = NodeEditorPinType_None
					// TODO: LINK INDEX
			};
			pin_index++;
		}

		ctx->pins[pin_index] =
			(NodeEditorPin){
				.index = pin_index,
				.node_index = n,
				.pos = (ImVec2){ node.pos.x + 90.0f, node.pos.y + 30.0f},
				.type = NodeEditorPinType_OutCtrl
				// TODO: LINK INDEX
		};
		pin_index++;

	}

	ctx->isDirty = false;
	//free(ctx->links);
}

void a3_NodeEditorAddNode(NodeEditorCtx* ctx, const char* title, a3i32 ctrl_count, a3i32 param_count) {
	ctx->isDirty = true;
	ctx->pins_count += ctrl_count + param_count + 1;

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
			.pin_count = ctrl_count + param_count,
			.title = title,
			.title_len = (a3ui32)strlen(title),
	};

	ctx->nodes = new_nodes;
	ctx->nodes_count++;
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

	for (a3i32 n = 0; n < ctx->nodes_count; n++) {
		a3_NodeEditorNode_Update(ctx, n);
	}
	for (a3i32 n = 0; n < ctx->pins_count; n++) {
		a3_NodeEditorPin_Update(ctx, n);
	}

	if (ctx->mouse_clicked) {
		ctx->node_selected = ctx->node_dragging = ctx->node_hovered;
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
	pin->pos = (ImVec2){ node.pos.x + 90.0f, node.pos.y + 30.0f };
}

void a3_NodeEditorPin_Update(NodeEditorCtx* ctx, a3i32 index) {
	NodeEditorPin* pin = ctx->pins + index;

	if (ctx->node_dragging == pin->node_index) {
		a3_NodeEditorPin_RefreshPos(ctx, index);
	}


}



void a3_NodeEditorPin_Draw(NodeEditorCtx* ctx, a3i32 index) {
	NodeEditorPin* pin = ctx->pins + index;
	ImU32 pin_color = IM_COL32(255, 255, 255, 255);


	ImVec2 center = (ImVec2){ pin->pos.x + ctx->canvas_origin.x, pin->pos.y + ctx->canvas_origin.y };


	ImDrawList_AddCircleFilled(ctx->CanvasDrawList, center, 5.0f, pin_color, 10);
	
	
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
	const ImU32 titlebar_color = IM_COL32(41, 74, 122, 255);
	const ImU32 outline_color = IM_COL32(200, 200, 200, 255);
	const ImU32 node_color = index != ctx->node_hovered ? nodebase_color : nodehover_color;



	a3vec2 offset = node->pos;
	a3real2Add(offset.v, ctx->canvas_origin.v);

	ImVec2 rect_min = (ImVec2){ offset.x, offset.y };
	ImVec2 rect_max = (ImVec2){ offset.x + 100.0f, offset.y + 100.0f };
	ImVec2 title_max = (ImVec2){ rect_max.x, offset.y + 20.0f };

	ImDrawList_AddRectFilled(ctx->CanvasDrawList, rect_min, rect_max, node_color, 10.0f, ImDrawFlags_RoundCornersAll);
	ImDrawList_AddRectFilled(ctx->CanvasDrawList, rect_min, title_max, titlebar_color, 10.0f, ImDrawFlags_RoundCornersTop);

	if (ctx->node_selected == index)
		ImDrawList_AddRect(ctx->CanvasDrawList, rect_min, rect_max, outline_color, 10.0f, ImDrawFlags_RoundCornersAll, 2.0f);

	ImVec2 title_start = (ImVec2){ offset.x + 10.0f, offset.y + 10.0f };
	ImDrawList_AddText_Vec2(ctx->CanvasDrawList, title_start, title_color, node->title, node->title + node->title_len);
}
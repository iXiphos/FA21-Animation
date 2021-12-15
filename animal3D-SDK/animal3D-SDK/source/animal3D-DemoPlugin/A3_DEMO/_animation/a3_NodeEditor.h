#ifndef __ANIMAL3D_NODE_EDITOR_H
#define __ANIMAL3D_NODE_EDITOR_H


#include <math.h>

#include "animal3D-A3DM/animal3D-A3DM.h"

#ifndef CIMGUI_DEFINE_ENUMS_AND_STRUCTS
#define CIMGUI_DEFINE_ENUMS_AND_STRUCTS 1
#endif
#undef true
#undef false
#include "cimgui/cimgui.h"


#include <stdlib.h>
#include <string.h>

/*
Much of this code was inspired by https://github.com/Nelarius/imnodes
to help with understanding how imgui works
*/

typedef struct NodeEditorCtx NodeEditorCtx;
typedef struct NodeEditorNode NodeEditorNode;
typedef struct NodeEditorPin NodeEditorPin;
typedef struct NodeEditorLink NodeEditorLink;
typedef enum NodeEditorPinType NodeEditorPinType;

#define nodetitlemaxlen 32

/*
Nodes are the actual operations
pins are the inputs and outputs
links are the lines between
*/

struct NodeEditorCtx {
	a3vec2 pan;

	a3vec2 canvas_origin;
	ImVec2 canvas_size;
	float zoom;

	ImDrawList* CanvasDrawList;
	/*
	
	list of nodes


	list of pins?
	list of links?

	*/
	a3i32 node_selected;
	a3i32 node_hovered;
	a3i32 node_dragging;

	a3i32 pin_selected;
	a3i32 pin_hovered;

	ImVec2 mouse_pos;
	ImVec2 mouse_delta;
	bool mouse_clicked;
	bool mouse_held;
	bool mouse_released;


	// should we regenerate our 
	bool isDirty;

	NodeEditorNode* nodes;
	a3i32 nodes_count;

	NodeEditorPin* pins;
	a3i32 pins_count;

	NodeEditorLink* links;
	a3i32 links_count;
};


struct NodeEditorNode {
	a3i32 index;
	a3vec2 pos;
	const char* title;
	a3ui32 title_len;
	a3i32 pin_count;

};

enum NodeEditorPinType {
	NodeEditorPinType_None,

	NodeEditorPinType_InCtrl = 1,
	NodeEditorPinType_OutCtrl = 2,

	NodeEditorPinType_InParam = 3,
	NodeEditorPinType_OutParam = 4,
};

struct NodeEditorPin {
	a3i32 index;
	ImVec2 pos;
	a3i32 node_index;
	a3i32 link_index;
	NodeEditorPinType type;
	//# of pin inside node
	a3ui8 pin_index;
};

struct NodeEditorLink {
	a3i32 index;
	a3vec2 start;
	a3vec2 end;
	a3i32 start_index;
	a3i32 end_index;
};



// rebuild our list of pins and links
// MUST BE CALLED BEFORE DOING ANYTHING WITH THESE LISTS IF ctx->isDirty
void a3_NodeEditorRebuildLists(NodeEditorCtx* ctx);
void a3_NodeEditorAddNode(NodeEditorCtx* ctx, const char* title, a3i32 ctrl_count, a3i32 param_count);

void a3_NodeEditorNode_Update(NodeEditorCtx* ctx, a3i32 index);
void a3_NodeEditorNode_Draw(NodeEditorCtx* ctx, a3i32 index);

void a3_NodeEditorPin_Update(NodeEditorCtx* ctx, a3i32 index);
void a3_NodeEditorPin_RefreshPos(NodeEditorCtx* ctx, a3i32 index);
void a3_NodeEditorPin_Draw(NodeEditorCtx* ctx, a3i32 index);

void a3_NodeEditorUpdate(NodeEditorCtx* ctx);

#endif
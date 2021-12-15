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
	animal3D SDK: Minimal 3D Animation Framework Extended (A3DX)
	By Daniel S. Buckstein
	
	a3_Hierarchy.c
	Hierarchy implementation.

	**DO NOT MODIFY THIS FILE**
*/

#include "../a3_Hierarchy.h"


#include <stdio.h>
#include <stdlib.h>
#include <string.h>


//-----------------------------------------------------------------------------

inline a3ret a3hierarchyInternalGetIndex(const a3_Hierarchy *hierarchy, const a3byte name[a3node_nameSize])
{
	a3ui32 i;
	for (i = 0; i < hierarchy->numNodes; ++i)
		if (!strncmp(hierarchy->nodes[i].name, name, a3node_nameSize))
			return i;
	return -1;
}

inline void a3hierarchyInternalSetNode(a3_HierarchyNode *node, const a3ui32 index, const a3i32 parentIndex, const a3byte name[a3node_nameSize])
{
	strncpy(node->name, name, a3node_nameSize);
	node->name[a3node_nameSize - 1] = 0;
	node->index = index;
	node->parentIndex = parentIndex;
}

// round up to nearest power of 2
// src: https://graphics.stanford.edu/~seander/bithacks.html#RoundUpPowerOf2
// 
a3ui32 a3getAllocSize(a3ui32 num) {
	num--;
	num |= num >> 1;
	num |= num >> 2;
	num |= num >> 4;
	num |= num >> 8;
	num |= num >> 16;
	num++;
	return num;
}

//-----------------------------------------------------------------------------

a3ret a3hierarchyCreate(a3_Hierarchy *hierarchy_out, const a3ui32 numNodes, const a3byte **names_opt)
{
	if (hierarchy_out && numNodes)
	{
		if (!hierarchy_out->nodes)
		{
			const a3ui32 dataSize = a3getAllocSize(sizeof(a3_HierarchyNode) * numNodes);
			const a3ui32 capacity = dataSize / sizeof(a3_HierarchyNode);
			
			a3ui32 i;
			const a3byte *tmpName;
			hierarchy_out->numNodes = numNodes;
			hierarchy_out->capacity = capacity;
			hierarchy_out->nodes = (a3_HierarchyNode *)malloc(dataSize);
			memset(hierarchy_out->nodes, 0, dataSize);
			

			if (names_opt)
			{
				for (i = 0; i < numNodes; ++i)
					if (tmpName = *(names_opt + i))
					{
						if (a3hierarchyInternalGetIndex(hierarchy_out, tmpName) < 0)
						{
							strncpy(hierarchy_out->nodes[i].name, tmpName, a3node_nameSize);
							hierarchy_out->nodes[i].name[a3node_nameSize - 1] = 0;
						}
						else
							printf("\n A3 Warning: Ignoring duplicate name string passed to hierarchy allocator.");
					}
					else
						printf("\n A3 Warning: Ignoring invalid name string passed to hierarchy allocator.");
			}
			return numNodes;
		}
	}
	return -1;
}

a3ret a3hierarchySetNode(const a3_Hierarchy *hierarchy, const a3ui32 index, const a3i32 parentIndex, const a3byte name[a3node_nameSize])
{
	a3_HierarchyNode *node;
	if (hierarchy)
	{
		if (hierarchy->nodes && index < hierarchy->numNodes)
		{
			if ((a3i32)index > parentIndex)
			{
				node = hierarchy->nodes + index;
				a3hierarchyInternalSetNode(node, index, parentIndex, name);
				return index;
			}
			else
				printf("\n A3 ERROR: Hierarchy node\'s index must be greater than its parent\'s.");
		}
	}
	return -1;
}

a3ret a3hierarchyGetNodeIndex(const a3_Hierarchy *hierarchy, const a3byte name[a3node_nameSize])
{
	if (hierarchy)
		if (hierarchy->nodes)
			return a3hierarchyInternalGetIndex(hierarchy, name);
	return -1;
}

a3ret a3hierarchyGetNodeNames(const a3byte *nameList_out[], const a3_Hierarchy *hierarchy)
{
	a3ui32 i;
	if (hierarchy && nameList_out)
	{
		if (hierarchy->nodes)
		{
			for (i = 0; i < hierarchy->numNodes; ++i)
				nameList_out[i] = hierarchy->nodes[i].name;
			return hierarchy->numNodes;
		}
	}
	return -1;
}

a3i32 a3hierarchyPrefixNodeNames(a3_Hierarchy const* hierarchy, a3byte const prefix[a3node_nameSize])
{
	if (hierarchy && hierarchy->nodes && prefix && *prefix)
	{
		a3byte copy[a3node_nameSize] = { 0 };
		a3ui32 i;
		for (i = 0; i < hierarchy->numNodes; ++i)
		{
			strncpy(copy, prefix, a3node_nameSize);
			strcat(copy, hierarchy->nodes[i].name);
			strncpy(hierarchy->nodes[i].name, copy, a3node_nameSize);
		}

		// done
		return i;
	}
	return 0;
}

a3ret a3hierarchySaveBinary(const a3_Hierarchy *hierarchy, const a3_FileStream *fileStream)
{
	FILE *fp;
	a3ui32 ret = 0;
	if (hierarchy && fileStream)
	{
		if (hierarchy->nodes)
		{
			fp = fileStream->stream;
			if (fp)
			{
				ret += (a3ui32)fwrite(&hierarchy->numNodes, 1, sizeof(a3ui32), fp);
				ret += (a3ui32)fwrite(hierarchy->nodes, 1, sizeof(a3_HierarchyNode) * hierarchy->numNodes, fp);
			}
			return ret;
		}
	}
	return -1;
}

a3ret a3hierarchyLoadBinary(a3_Hierarchy *hierarchy, const a3_FileStream *fileStream)
{
	FILE *fp;
	a3ui32 ret = 0;
	a3ui32 dataSize = 0;
	if (hierarchy && fileStream)
	{
		if (!hierarchy->nodes)
		{
			fp = fileStream->stream;
			if (fp)
			{
				ret += (a3ui32)fread(&hierarchy->numNodes, 1, sizeof(a3ui32), fp);

				dataSize = sizeof(a3_HierarchyNode) * hierarchy->numNodes;
				hierarchy->nodes = (a3_HierarchyNode *)malloc(dataSize);
				ret += (a3ui32)fread(hierarchy->nodes, 1, dataSize, fp);
			}
			return ret;
		}
	}
	return -1;
}

a3ret a3hierarchyCopyToString(const a3_Hierarchy *hierarchy, a3byte *str)
{
	const a3byte *const start = str;
	if (hierarchy && str)
	{
		if (hierarchy->nodes)
		{
			str = (a3byte *)((a3ui32 *)memcpy(str, &hierarchy->numNodes, sizeof(a3ui32)) + 1);
			str = (a3byte *)((a3_HierarchyNode *)memcpy(str, hierarchy->nodes, sizeof(a3_HierarchyNode) * hierarchy->numNodes) + hierarchy->numNodes);

			// done
			return (a3i32)(str - start);
		}
	}
	return -1;
}

a3ret a3hierarchyCopyFromString(a3_Hierarchy *hierarchy, const a3byte *str)
{
	const a3byte *const start = str;
	a3ui32 dataSize = 0;
	if (hierarchy && str)
	{
		if (!hierarchy->nodes)
		{
			memcpy(&hierarchy->numNodes, str, sizeof(a3ui32));
			str += sizeof(a3ui32);

			dataSize = sizeof(a3_HierarchyNode) * hierarchy->numNodes;
			hierarchy->nodes = (a3_HierarchyNode *)malloc(dataSize);
			memcpy(hierarchy->nodes, str, dataSize);
			str += dataSize;

			// done
			return (a3i32)(str - start);
		}
	}
	return -1;
}

a3ret a3hierarchyGetStringSize(const a3_Hierarchy *hierarchy)
{
	if (hierarchy)
	{
		if (hierarchy->nodes)
		{
			const a3ui32 dataSize
				= sizeof(a3ui32)
				+ sizeof(a3_HierarchyNode) * hierarchy->numNodes;
			return dataSize;
		}
	}
	return -1;
}

a3ret a3hierarchyRelease(a3_Hierarchy *hierarchy)
{
	if (hierarchy)
	{
		if (hierarchy->nodes)
		{
			free(hierarchy->nodes);
			hierarchy->nodes = 0;
			hierarchy->numNodes = 0;
			return 1;
		}
	}
	return -1;
}
/*
// input is unsorted hierarchy
// output is sorted hierarchy
a3ret a3hierarchyFixOrder(a3_Hierarchy* hierarchy_inout) {
	
	const a3ui32 numNodes = hierarchy_inout->numNodes;
	const a3_HierarchyNode* nodesStart = hierarchy_inout->nodes;
	const a3_HierarchyNode* nodesEnd = nodesStart + numNodes;

	a3_HierarchyNode* out_nodes = (a3_HierarchyNode*)malloc(numNodes * sizeof(a3_HierarchyNode));
	
	a3_HierarchyNode* root = NULL;

	// find root node (should be first node but just incase)
	for (a3_HierarchyNode* itr = nodesStart; itr < nodesEnd; itr++) {
		if (itr->parentIndex == -1) {
			root = itr;
			break;
		}
	}


	if (root == NULL) 
		// missing root node
		return -1;


	a3ui32 outIndex = 0;
	
	// out list maintains its "index" from original list until the end to make finding children easier
}
*/


a3ret a3hierarchyExpand(a3_Hierarchy* hierarchy) {
	const a3ui32 oldDataSize = hierarchy->numNodes * sizeof(a3_HierarchyNode);
	const a3ui32 newDataSize = a3getAllocSize(oldDataSize) * 2; // get the next power of two
	const a3ui32 newCapacity = newDataSize / sizeof(a3_HierarchyNode);

	a3_HierarchyNode* newNodes = (a3_HierarchyNode*)malloc(newDataSize);
	memcpy(newNodes, hierarchy->nodes, oldDataSize);
	free(hierarchy->nodes);
	hierarchy->nodes = newNodes;
	hierarchy->capacity = newCapacity;

	return 1;
}

a3ret a3hierarchyAppend(a3_Hierarchy* hierarchy, const a3i32 parentIndex, const a3byte* name) {

	// we assume that the hierarchy is not broken
	if (hierarchy->numNodes >= hierarchy->capacity)
		a3hierarchyExpand(hierarchy);
	
	const a3i32 index = hierarchy->numNodes;
	a3_HierarchyNode* node = hierarchy->nodes + index;
	node->index = index;
	node->childCount = 0;
	node->parentIndex = parentIndex;
	a3hierarchySetName(hierarchy, index, name);

	if (parentIndex != -1)
		hierarchy->nodes[parentIndex].childCount++;
	
	return 1;
}

a3ret a3hierarchySetName(a3_Hierarchy* hierarchy, a3i32 index, const a3byte* name) {
	strncpy(hierarchy->nodes[index].name, name, a3node_nameSize);
	return 1;
}


a3ret a3hierarchyUpdateChildCount(a3_Hierarchy* hierarchy) {
	for (a3ui32 i = 0; i < hierarchy->numNodes; i++) {
		hierarchy->nodes[i].childCount = 0;
	}
	for (a3ui32 i = 0; i < hierarchy->numNodes; i++) {
		a3_HierarchyNode* node = hierarchy->nodes + i;
		if (node->parentIndex != -1) {
			hierarchy->nodes[node->parentIndex].childCount++;
		}
	}
	return 1;
}

const a3byte* a3hierarchyGetName(const a3_Hierarchy* hierarchy, a3i32 index) {
	return hierarchy->nodes[index].name;
}

// remove node and children from hierarchy, create new hierarchy with node as root 
a3ret a3hierarchySplit(a3_Hierarchy* src, a3_Hierarchy* dst, a3i32 index) {

	return -1;
}

// append all nodes of src as child of node in dst
a3ret a3hierarchyJoin(const a3_Hierarchy* src, a3_Hierarchy* dst, a3i32 parentIndex) {

	if (parentIndex == -1)
		return -1;

	dst->nodes[parentIndex].childCount++;

	const a3ui32 capacityNeeded = src->numNodes + dst->numNodes;
	const a3ui32 indexOffset = src->numNodes;

	while (dst->capacity < capacityNeeded) {
		a3hierarchyExpand(dst);
	}

	const a3_HierarchyNode* srcNode = src->nodes;
	a3_HierarchyNode* dstNode = dst->nodes;

	const a3_HierarchyNode* srcEnd = src->nodes + src->numNodes;

	for (; srcNode < srcEnd; dstNode++, srcNode++) {
		*dstNode = *srcNode;
		dstNode->index += indexOffset;
		dstNode->parentIndex += indexOffset;
	}

	return 1;
}

//-----------------------------------------------------------------------------

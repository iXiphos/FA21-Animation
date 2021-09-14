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
	
	passTangentBasis_morph5_transform_vs4x.glsl
	Calculate and pass tangent basis with morphing.
*/

#version 450

struct sMorphTarget
{
	vec4 position, normal, tangent;
};



layout (location = 0) in sMorphTarget aMorphTarget[5];
/*
layout (location = 0) in vec4 aPosition;
layout (location = 1) in vec4 aNormal;		// usually 2
layout (location = 2) in vec4 aTangent;		// usually 10
layout (location = 2) in vec4 aBitangent;	// usually 11
*/
layout (location = 15) in vec4 aTexcoord;	// usually 8



uniform mat4 uP;
uniform mat4 uMV, uMV_nrm;
uniform mat4 uAtlas;
uniform double uTime;

out vbVertexData {
	mat4 vTangentBasis_view;
	vec4 vTexcoord_atlas;
};

flat out int vVertexID;
flat out int vInstanceID;

vec4 lerp(in vec4 v0, in vec4 v1, in float u);
vec4 nlerp(in vec4 v0, in vec4 v1, in float u);
vec4 CatmullRom(in vec4 vP, in vec4 v0, in vec4 v1, in vec4 vN, in float u);
vec4 nCatmullRom(in vec4 vP, in vec4 v0, in vec4 v1, in vec4 vN, in float u);

sMorphTarget lerp(in sMorphTarget m0, in sMorphTarget m1, in float u)
{
	sMorphTarget m;
	m.position = lerp(m0.position, m1.position, u);
	m.normal = nlerp(m0.normal, m1.normal, u);
	m.tangent = nlerp(m0.tangent, m1.tangent, u);

	return m;
}

sMorphTarget CatmullRom(in sMorphTarget vP, in sMorphTarget v0, in sMorphTarget v1, in sMorphTarget vN, in float u)
{
	sMorphTarget m;
	m.position = CatmullRom(vP.position, v0.position, v1.position, vN.position, u);
	m.normal = nCatmullRom(vP.normal, v0.normal, v1.normal, vN.normal, u);
	m.tangent = nCatmullRom(vP.tangent, v0.tangent, v1.tangent, vN.tangent, u);
	return m;
}

void main()
{
	// DUMMY OUTPUT: directly assign input position to output position
//	gl_Position = aPosition;

	float t = float(uTime * 1.5);
	float u = fract(t);
	int i0 = int(t) % 5;
	int i1 = (i0 + 1) % 5;
	int iN = (i1 + 1) % 5;
	int iP = (i0 + 4) % 5;

	sMorphTarget k;
	sMorphTarget k0 = aMorphTarget[i0];
	sMorphTarget k1 = aMorphTarget[i1];

	sMorphTarget kN = aMorphTarget[iN];
	sMorphTarget kP = aMorphTarget[iP];

	k = aMorphTarget[i0];

	//Step
	k = k0;

	//nearest
	k = u < 0.5 ? k : k1;

	//lerp
	k = lerp(k0, k1, u);

	//CatmullRom
	k = CatmullRom(kP, k0, k1, kN, u);

	vTangentBasis_view = uMV_nrm * mat4(
	k.tangent, 
	vec4(cross(k.normal.xyz, k.tangent.xyz), 0.0), 
	k.normal, 
	vec4(0.0));
	
	vTangentBasis_view[3] = uMV * k.position;

	//vTangentBasis_view = uMV_nrm * mat4(aTangent, aBitangent, aNormal, vec4(0.0));
	//vTangentBasis_view[3] = uMV * aPosition;


	gl_Position = uP * vTangentBasis_view[3];
	
	vTexcoord_atlas = uAtlas * aTexcoord;

	vVertexID = gl_VertexID;
	vInstanceID = gl_InstanceID;
}

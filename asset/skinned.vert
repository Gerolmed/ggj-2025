#version 100 

precision highp float;

attribute vec3 vertexPosition;
attribute vec3 vertexNormal;
attribute vec2 vertexTexCoord;
attribute vec4 vertexColor;
attribute vec4 vertexBoneIds;
attribute vec4 vertexBoneWeights;

varying vec2 uv;
varying vec3 normal;
varying vec3 pos_view;
varying vec4 color;

uniform mat4 mvp;
uniform mat4 matModel;
uniform mat4 matView;

uniform mat4 boneMatrices[128];

void main() 
{
	vec4 pos = vec4(vertexPosition, 1.0);

    int boneIndex0 = int(vertexBoneIds.x);
    int boneIndex1 = int(vertexBoneIds.y);
    int boneIndex2 = int(vertexBoneIds.z);
    int boneIndex3 = int(vertexBoneIds.w);

	mat4 bone = boneMatrices[boneIndex0] * vertexBoneWeights.r;
	bone += boneMatrices[boneIndex1] * vertexBoneWeights.g;
	bone += boneMatrices[boneIndex2] * vertexBoneWeights.b;
	bone += boneMatrices[boneIndex3] * vertexBoneWeights.w;

    pos_view = (matView * matModel * pos).xyz;
	uv = vertexTexCoord;
	normal = (matModel * bone * vec4(vertexNormal, 0.0)).xyz;
    color = vertexColor;

	gl_Position = mvp * bone * pos;
}   


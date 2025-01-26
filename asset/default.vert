#version 100

precision highp float;

attribute vec3 vertexPosition;
attribute vec2 vertexTexCoord; 
attribute vec3 vertexNormal;
attribute vec4 vertexColor;

varying vec2 uv;
varying vec3 normal;
varying vec3 pos_view;
varying vec3 pos_world;
varying vec4 color;

uniform mat4 mvp;
uniform mat4 matModel;
uniform mat4 matView;

void main() 
{
	vec4 pos = vec4(vertexPosition, 1.0);

    pos_view = (matView * matModel * pos).xyz;
    pos_world = (matModel * pos).xyz;
	uv = vertexTexCoord;
	normal = (matModel * vec4(vertexNormal, 0.0)).xyz;
    color = vertexColor;

	gl_Position = mvp * pos;
}   


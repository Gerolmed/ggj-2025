#version 100

precision highp float;

varying vec2 uv; 
varying vec3 normal;
varying vec3 pos_view;
varying vec4 color;

uniform sampler2D texture0;
uniform vec4 colDiffuse;


void main()
{
	vec4 texelColor = texture2D(texture0, uv);

    vec3 n = normalize(normal);
    float test = dot(n, pos_view); 
	gl_FragColor = vec4(test, test, test, 0.8);
}  


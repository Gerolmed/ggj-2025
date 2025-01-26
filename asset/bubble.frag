#version 100

precision highp float;

varying vec2 uv; 
varying vec3 normal;
varying vec3 pos_view;
varying vec3 pos_world;
varying vec4 color;

uniform sampler2D texture0;
uniform vec4 colDiffuse;


float fresnel(vec3 a, vec3 b)
{
    return pow(1.0 - clamp(dot(a, b), 0.0, 1.0) * 0.25, 5.0);
}

void main()
{
	vec4 texelColor = texture2D(texture0, uv);

    vec3 camera_pos = vec3(-10, 10, 0);

    vec3 n = normalize(normal);
    vec3 v = normalize(camera_pos - pos_world);
    float test = fresnel(n, v);

    vec3 inner_color = vec3(0.4, 0.5, 0.8);
    // vec3 outer_color = vec3(0.6, 0.8, 1.1);
    vec3 outer_color = vec3(66.0 / 255.0, 191.0 / 255.0, 232.0 / 255.0);
    // vec3 outer_color = vec3(144.0 / 255.0, 82.0 / 255.0, 188.0 / 255.0);

    vec3 color = mix(outer_color, inner_color, test);

	gl_FragColor = vec4(color, 0.9);
}  


#version 100

precision highp float;

varying vec2 uv; 
varying vec3 normal;
varying vec3 pos_view;
varying vec4 color;

uniform sampler2D texture0;
uniform vec4 colDiffuse;


const vec3 sun_dir = normalize(vec3(-1, 2.0, 0.0));
const vec3 sun_color = vec3(0.8, 0.9, 1.1);

const float to_radians = 3.14 / 180.0;

float lumen(vec3 color)
{
    return dot(color, vec3(0.299, 0.587, 0.114));
}

void main()
{
	vec3 n = normalize(normal);

	vec4 texelColor = texture2D(texture0, uv);
    // texelColor.rgb = pow(texelColor.rgb, vec3(2.2));
	
    gl_FragColor  = texelColor * color * vec4(colDiffuse.rgb, 1.0);

    if (gl_FragColor.a < 0.1)
    {
        discard;
    }

	vec3 light = vec3(0.1);
    light += clamp(dot(sun_dir, n), 0.0, 1.0);

    float lum = lumen(light);
    vec3 t = 0.9f * sun_color;
    if (lum > 0.6f) 
    {
        t = 1.1 * sun_color;
    }
    if (lum > 1.0f)
    {
        t = 1.3 * sun_color;
    }
	gl_FragColor.rgb *= t;

    // gl_FragColor.rgb = n;
}  


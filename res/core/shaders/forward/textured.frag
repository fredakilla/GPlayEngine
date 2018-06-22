#ifdef OPENGL_ES
#ifdef GL_FRAGMENT_PRECISION_HIGH
precision highp float;
#else
precision mediump float;
#endif
#endif

#ifndef DIRECTIONAL_LIGHT_COUNT
#define DIRECTIONAL_LIGHT_COUNT 0
#endif
#ifndef SPOT_LIGHT_COUNT
#define SPOT_LIGHT_COUNT 0
#endif
#ifndef POINT_LIGHT_COUNT
#define POINT_LIGHT_COUNT 0
#endif
#if (DIRECTIONAL_LIGHT_COUNT > 0) || (POINT_LIGHT_COUNT > 0) || (SPOT_LIGHT_COUNT > 0)
#define LIGHTING
#endif

///////////////////////////////////////////////////////////
// Uniforms
uniform vec4 u_ambientColor;

uniform sampler2D u_diffuseTexture;

#if defined(LIGHTMAP)
uniform sampler2D u_lightmapTexture;
#endif

#if defined(LIGHTING)

#if defined(BUMPED)
uniform sampler2D u_normalmapTexture;
#endif

#if (DIRECTIONAL_LIGHT_COUNT > 0)
uniform vec4 u_directionalLightColor[DIRECTIONAL_LIGHT_COUNT];
#if !defined(BUMPED)
uniform vec4 u_directionalLightDirection[DIRECTIONAL_LIGHT_COUNT];
#endif
#endif

#if (POINT_LIGHT_COUNT > 0)
uniform vec4 u_pointLightColor[POINT_LIGHT_COUNT];
uniform vec4 u_pointLightPosition[POINT_LIGHT_COUNT];
uniform vec4 u_pointLightRangeInverse[POINT_LIGHT_COUNT];
#endif

#if (SPOT_LIGHT_COUNT > 0)
uniform vec4 u_spotLightColor[SPOT_LIGHT_COUNT];
uniform vec4 u_spotLightRangeInverse[SPOT_LIGHT_COUNT];
uniform vec4 u_spotLightInnerAngleCos[SPOT_LIGHT_COUNT];
uniform vec4 u_spotLightOuterAngleCos[SPOT_LIGHT_COUNT];
#if !defined(BUMPED)
uniform vec4 u_spotLightDirection[SPOT_LIGHT_COUNT];
#endif
#endif

#if defined(SPECULAR)
uniform vec4 u_specularExponent;
#endif

#endif

#if defined(MODULATE_COLOR)
uniform vec4 u_modulateColor;
#endif

#if defined(MODULATE_ALPHA)
uniform vec4 u_modulateAlpha;
#endif

///////////////////////////////////////////////////////////
// Variables
vec4 _baseColor;

///////////////////////////////////////////////////////////
// Varyings
varying vec2 v_texcoord0;

#if defined(LIGHTMAP)
varying vec2 v_texcoord1;
#endif

#if defined(LIGHTING)

#if !defined(BUMPED)
varying vec3 v_normalVector;
#endif

#if defined(BUMPED) && (DIRECTIONAL_LIGHT_COUNT > 0)
varying vec3 v_directionalLightDirection[DIRECTIONAL_LIGHT_COUNT];
#endif

#if (POINT_LIGHT_COUNT > 0)
varying vec3 v_vertexToPointLightDirection[POINT_LIGHT_COUNT];
#endif

#if (SPOT_LIGHT_COUNT > 0)
varying vec3 v_vertexToSpotLightDirection[SPOT_LIGHT_COUNT];
#if defined(BUMPED)
varying vec3 v_spotLightDirection[SPOT_LIGHT_COUNT];
#endif
#endif

#if defined(SPECULAR)
varying vec3 v_cameraDirection; 
#endif

#include "lighting.frag"

#endif

#if defined(CLIP_PLANE)
varying float v_clipDistance;
#endif


void main()
{
    #if defined(CLIP_PLANE)
    if(v_clipDistance < 0.0) discard;
    #endif
 
    _baseColor = texture2D(u_diffuseTexture, v_texcoord0);
 
    gl_FragColor.a = _baseColor.a;

    #if defined(TEXTURE_DISCARD_ALPHA)
    if (gl_FragColor.a < 0.5)
        discard;
    #endif

    #if defined(LIGHTING)

    gl_FragColor.rgb = getLitPixel();
    #else
    gl_FragColor.rgb = _baseColor.rgb;
    #endif

	#if defined(LIGHTMAP)
	vec4 lightColor = texture2D(u_lightmapTexture, v_texcoord1);
	gl_FragColor.rgb *= lightColor.rgb;
	#endif

    #if defined(MODULATE_COLOR)
    gl_FragColor *= u_modulateColor;
    #endif

    #if defined(MODULATE_ALPHA)
    gl_FragColor.a *= u_modulateAlpha.x;
    #endif
}

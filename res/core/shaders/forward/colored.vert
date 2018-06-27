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
// Attributes
attribute vec4 a_position;

#if defined(SKINNING)
attribute vec4 a_weight;
attribute vec4 a_indices;
#endif

#if defined(LIGHTMAP)
attribute vec2 a_texcoord1;
#endif

#if defined(LIGHTING)
attribute vec3 a_normal;
#endif

#if defined(VERTEX_COLOR)
attribute vec3 a_color0;
#endif

#if defined(INSTANCED)
attribute vec4 i_data0;
attribute vec4 i_data1;
attribute vec4 i_data2;
attribute vec4 i_data3;
#endif

///////////////////////////////////////////////////////////
// Uniforms
uniform mat4 u_worldViewProjectionMatrix;

#if defined(SKINNING)
uniform vec4 u_matrixPalette[SKINNING_JOINT_COUNT * 3];
#endif

#if defined(LIGHTING)
uniform mat4 u_inverseTransposeWorldViewMatrix;

#if (POINT_LIGHT_COUNT > 0) || (SPOT_LIGHT_COUNT > 0) || defined(SPECULAR)
uniform mat4 u_worldViewMatrix;
#endif

#if (DIRECTIONAL_LIGHT_COUNT > 0)
uniform vec4 u_directionalLightDirection[DIRECTIONAL_LIGHT_COUNT];
#endif

#if (POINT_LIGHT_COUNT > 0) 
uniform vec4 u_pointLightPosition[POINT_LIGHT_COUNT];
#endif

#if (SPOT_LIGHT_COUNT > 0)
uniform vec4 u_spotLightPosition[SPOT_LIGHT_COUNT];
uniform vec4 u_spotLightDirection[SPOT_LIGHT_COUNT];
#endif

#if defined(SPECULAR)
uniform vec4 u_cameraPosition;
#endif

#endif

#if defined(CLIP_PLANE)
uniform mat4 u_worldMatrix;
uniform vec4 u_clipPlane;
#endif

///////////////////////////////////////////////////////////
// Varyings
#if defined(LIGHTMAP)
varying vec2 v_texcoord1;
#endif

#if defined(VERTEX_COLOR)
varying vec3 v_color0;
#endif

#if defined(LIGHTING)

varying vec3 v_normalVector;

#if (DIRECTIONAL_LIGHT_COUNT > 0) 
varying vec3 v_lightDirection[DIRECTIONAL_LIGHT_COUNT];
#endif

#if (POINT_LIGHT_COUNT > 0)
varying vec3 v_vertexToPointLightDirection[POINT_LIGHT_COUNT];
#endif

#if (SPOT_LIGHT_COUNT > 0)
varying vec3 v_vertexToSpotLightDirection[SPOT_LIGHT_COUNT];
#endif

#if defined(SPECULAR)
varying vec3 v_cameraDirection;
#endif

#include "lighting.vert"

#endif

#if defined(SKINNING)
#include "skinning.vert"
#else
#include "skinning-none.vert" 
#endif

#if defined(CLIP_PLANE)
varying float v_clipDistance;
#endif

void main()
{
    vec4 position = getPosition();

#if defined (INSTANCED)
    mat4 model;
    model[0] = i_data0;
    model[1] = i_data1;
    model[2] = i_data2;
    model[3] = i_data3;
    vec4 worldPos = model * position;
    gl_Position = u_worldViewProjectionMatrix * worldPos;
#else
    gl_Position = u_worldViewProjectionMatrix * position;
#endif

    #if defined (LIGHTING)

    vec3 normal = getNormal();

    // Transform normal to view space.
    mat3 inverseTransposeWorldViewMatrix = mat3(u_inverseTransposeWorldViewMatrix[0].xyz, u_inverseTransposeWorldViewMatrix[1].xyz, u_inverseTransposeWorldViewMatrix[2].xyz);
    v_normalVector = inverseTransposeWorldViewMatrix * normal;

    // Apply light.
    applyLight(position);

    #endif

    // Pass the lightmap texture coordinate
    #if defined(LIGHTMAP)
    v_texcoord1 = a_texcoord1;
    #endif
    
    // Pass the vertex color
    #if defined(VERTEX_COLOR)
	v_color0 = a_color0;
    #endif
    
    #if defined(CLIP_PLANE)
    v_clipDistance = dot(u_worldMatrix * position, u_clipPlane);
    #endif    
}

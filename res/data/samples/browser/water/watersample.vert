/////////////////////////////
// Attributes
attribute vec4 a_position;
attribute vec2 a_texcoord0;

/////////////////////////////
// Uniforms
uniform mat4 u_worldMatrix;
uniform mat4 u_worldViewProjectionMatrix;
uniform mat4 u_worldViewProjectionReflectionMatrix;
uniform vec4 u_cameraPosition;

/////////////////////////////
// Varyings
varying vec4 v_vertexRefractionPosition;
varying vec4 v_vertexReflectionPosition;
varying vec2 v_texCoord;
varying vec3 v_eyePosition;


void main()
{
    v_vertexRefractionPosition = u_worldViewProjectionMatrix * a_position;
    v_vertexReflectionPosition = u_worldViewProjectionReflectionMatrix * a_position;

    gl_Position = v_vertexRefractionPosition;

    v_texCoord = a_texcoord0;

    v_eyePosition = u_cameraPosition.xyz - (u_worldMatrix * a_position).xyz;
}
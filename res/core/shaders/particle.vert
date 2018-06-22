///////////////////////////////////////////////////////////
// Attributes
attribute vec3 a_position;
attribute vec4 a_color0;
attribute vec2 a_texcoord0;

///////////////////////////////////////////////////////////
// Uniforms
uniform mat4 u_worldViewProjectionMatrix;

///////////////////////////////////////////////////////////
// Varyings
varying vec2 v_texCoord;
varying vec4 v_color;


void main()
{
    gl_Position = u_worldViewProjectionMatrix * vec4(a_position, 1);
    v_texCoord = vec2(a_texcoord0.x, 1.0 - a_texcoord0.y);
    v_color = a_color0;
}

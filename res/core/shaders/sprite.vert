///////////////////////////////////////////////////////////
// Attributes
attribute vec3 a_position;
attribute vec2 a_texcoord0;
attribute vec4 a_color0;

///////////////////////////////////////////////////////////
// Uniforms
uniform mat4 u_projectionMatrix;

///////////////////////////////////////////////////////////
// Varyings
varying vec2 v_texcoord0;
varying vec4 v_color0;


void main()
{
    gl_Position = u_projectionMatrix * vec4(a_position, 1);
    v_texcoord0 = vec2(a_texcoord0.x, 1.0 - a_texcoord0.y);
    v_color0 = a_color0;
}

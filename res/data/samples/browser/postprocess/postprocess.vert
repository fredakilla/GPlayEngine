// Inputs
attribute vec2 a_position;
attribute vec2 a_texcoord0;

// Varying
varying vec2 v_texcoord0;

void main()
{
    gl_Position = vec4(a_position, 0, 1);
    v_texcoord0 = a_texcoord0;
}

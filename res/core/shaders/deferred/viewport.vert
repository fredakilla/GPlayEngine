attribute vec4 a_position;
attribute vec2 a_texcoord0;


uniform mat4 u_worldViewProjectionMatrix;
uniform mat4 u_projectionMatrix;

varying vec4 v_position;
varying vec2 v_texcoord0;

void main()
{
    //gl_Position = u_projectionMatrix * a_position;
    //v_texcoord0 = vec2(a_texcoord0.x, 1.0 - a_texcoord0.y);

    gl_Position = vec4(a_position.xy, 0.0, 1.0);
    v_texcoord0 = a_texcoord0;
}

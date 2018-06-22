///////////////////////////////////////////////////////////
// Attributes
attribute vec3 a_position;
attribute vec2 a_texcoord0;

///////////////////////////////////////////////////////////
// Uniforms
uniform mat4 u_projectionMatrix;

///////////////////////////////////////////////////////////
// Varyings
varying vec2 v_texCoord;


void main()
{

    gl_Position = u_projectionMatrix * vec4( a_position.xy, 0, 1);
    v_texCoord = vec2(a_texcoord0.x, 1.0 - a_texcoord0.y);
    //v_texCoord = 1.0 - a_texcoord0;



    //gl_Position = u_projectionMatrix * vec4( a_position.xy, 0, 1);
    //v_texCoord = a_texcoord0;
}

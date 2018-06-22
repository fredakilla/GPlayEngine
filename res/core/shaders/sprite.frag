#ifdef OPENGL_ES
#ifdef GL_FRAGMENT_PRECISION_HIGH
precision highp float;
#else
precision mediump float;
#endif
#endif

///////////////////////////////////////////////////////////
// Uniforms
uniform sampler2D u_texture;

///////////////////////////////////////////////////////////
// Varyings
varying vec2 v_texcoord0;
varying vec4 v_color0;


void main()
{
    gl_FragColor = v_color0 * texture2D(u_texture, v_texcoord0);
}
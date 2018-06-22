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



void main()
{
    gl_FragColor = vec4(1.0, 0.0, 1.0, 1.0);
}

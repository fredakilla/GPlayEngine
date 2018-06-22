#ifdef OPENGL_ES
precision mediump float;
#endif

// Uniforms
uniform sampler2D u_texture;
uniform vec4 u_length;
#define LENGTH u_length.x 

// Inputs
varying vec2 v_texcoord0;

void main()
{
    vec4 sum = vec4(0.0);
 
   // blur in y (vertical) take nine samples, with the distance LENGTH between them
   sum += texture2D(u_texture, vec2(v_texcoord0.x - 4.0 * LENGTH, v_texcoord0.y)) * 0.05;
   sum += texture2D(u_texture, vec2(v_texcoord0.x - 3.0 * LENGTH, v_texcoord0.y)) * 0.09;
   sum += texture2D(u_texture, vec2(v_texcoord0.x - 2.0 * LENGTH, v_texcoord0.y)) * 0.12;
   sum += texture2D(u_texture, vec2(v_texcoord0.x -       LENGTH, v_texcoord0.y)) * 0.15;
   sum += texture2D(u_texture, vec2(v_texcoord0.x, v_texcoord0.y)) * 0.16;
   sum += texture2D(u_texture, vec2(v_texcoord0.x +       LENGTH, v_texcoord0.y)) * 0.15;
   sum += texture2D(u_texture, vec2(v_texcoord0.x + 2.0 * LENGTH, v_texcoord0.y)) * 0.12;
   sum += texture2D(u_texture, vec2(v_texcoord0.x + 3.0 * LENGTH, v_texcoord0.y)) * 0.09;
   sum += texture2D(u_texture, vec2(v_texcoord0.x + 4.0 * LENGTH, v_texcoord0.y)) * 0.05;
 
   gl_FragColor = sum;
}
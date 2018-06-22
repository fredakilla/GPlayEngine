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
varying vec2 v_texCoord;


float LinearizeDepth(in vec2 uv)
{
    float zNear = 0.5;    // TODO: Replace by the zNear of your perspective projection
    float zFar  = 10.0; // TODO: Replace by the zFar  of your perspective projection
    float depth = texture2D(u_texture, uv).x;
    return (2.0 * zNear) / (zFar + zNear - depth * (zFar - zNear));
}


void main()
{
    vec4 color = vec4(1.0, 1.0, 1.0, 1.0);

#ifdef SHOW_TEXTURE
    color = texture2D(u_texture, v_texCoord);
#endif

#ifdef SHOW_DEPTH
    float c = LinearizeDepth(v_texCoord);
    color = vec4(c, c, c, 1.0);
#endif

    gl_FragColor = color;

}

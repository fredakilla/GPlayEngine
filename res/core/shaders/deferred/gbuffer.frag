
varying vec3 v_normal;
varying vec2 v_texcoord0;

uniform sampler2D u_diffuseTexture;
uniform sampler2D u_specularTexture;


#if defined(NORMAL_MAP)
    uniform sampler2D u_normalTexture;
    varying mat3 v_tbnMatrix;
#endif


void main()
{
    // diffuse + specular
    gl_FragData[0].rgb = texture2D(u_diffuseTexture, v_texcoord0).rgb;
    gl_FragData[0].a = texture2D(u_specularTexture, v_texcoord0).r;

    // normal
#if defined(NORMAL_MAP)
    vec3 normalMap = texture2D(u_normalTexture, v_texcoord0).rgb * 2.0 - 1.0;
    normalMap = v_tbnMatrix * normalMap;
    gl_FragData[1] = vec4(normalMap, 1.0);
#else
    gl_FragData[1] = vec4(normalize(v_normal), 1.0);
#endif

}

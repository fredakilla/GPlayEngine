attribute vec4 a_position;
attribute vec3 a_normal;
attribute vec2 a_texcoord0;
attribute vec3 a_tangent;
attribute vec3 v_binormal;


uniform mat4 u_worldViewProjectionMatrix;
uniform mat4 u_worldMatrix;
uniform mat4 u_inverseTransposeWorldMatrix;


varying vec3 v_normal;
varying vec2 v_texcoord0;
#if defined(NORMAL_MAP)
    varying mat3 v_tbnMatrix;
#endif


void main()
{
    vec4 worldPos = u_worldMatrix * vec4(a_position.xyz, 1.0);
    //v_texcoord0 = a_texcoord0;
    v_texcoord0 = vec2(a_texcoord0.x, 1.0 - a_texcoord0.y);
    
    //mat3 normalMatrix = transpose(inverse(mat3(u_worldMatrix)));
    mat3 normalMatrix = mat3(u_inverseTransposeWorldMatrix);
    v_normal = normalMatrix * a_normal;

#if defined(NORMAL_MAP)
    vec3 N = normalize(normalMatrix * a_normal);
    vec3 T = normalize(normalMatrix * a_tangent);
    vec3 B = cross(T,N);
    v_tbnMatrix = (mat3(T,B,N));
#endif

    gl_Position = u_worldViewProjectionMatrix * vec4(a_position.xyz, 1.0);
}

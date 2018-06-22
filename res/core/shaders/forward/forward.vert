$input a_position, a_normal, a_texcoord0, a_tangent, a_bitangent
$output v_color, v_texcoord0, v_normal, v_position, v_tbnViewSpace, v_shadowcoord

#include "../common/bgfx_shader.sh"


#define BUMP_MAP

uniform mat4 u_worldViewMatrix;
uniform mat4 u_worldViewProjectionMatrix;
uniform mat4 u_inverseTransposeWorldViewMatrix;
uniform mat4 u_worldMatrix;

uniform mat4 u_lightSpaceMatrix;

void main()
{
    vec4 position = u_worldViewProjectionMatrix * a_position;
    v_position = u_worldViewMatrix * a_position;   

    mat3 normalMatrix = mat3(u_inverseTransposeWorldViewMatrix);
    v_normal = normalMatrix * a_normal;	

    v_texcoord0 = vec2(a_texcoord0.x, 1.0 - a_texcoord0.y);
	

#if defined(BUMP_MAP)
	mat3 inverseTransposeWorldViewMatrix = mat3(u_inverseTransposeWorldViewMatrix);
	vec3 N = normalize(inverseTransposeWorldViewMatrix * a_normal);
	vec3 T = normalize(inverseTransposeWorldViewMatrix * a_tangent);
    vec3 B = cross(T,N);
    v_tbnViewSpace = mat3(T,B,N);
#endif

	
	// shadow
	const float shadowMapOffset = 0.005;
	vec3 posOffset = a_position.xyz + v_normal.xyz * shadowMapOffset;
	//v_shadowcoord = mul(u_lightSpaceMatrix, mul(u_worldMatrix, vec4(posOffset, 1.0)) );
	v_shadowcoord = u_lightSpaceMatrix * u_worldMatrix * a_position;


    gl_Position = position;
}

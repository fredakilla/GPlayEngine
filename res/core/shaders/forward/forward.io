vec3 v_normal       : NORMAL    = vec3(0.0, 0.0, 1.0);
vec2 v_texcoord0    : TEXCOORD0 = vec2(0.0, 0.0);
vec3 v_color        : COLOR0 = vec3(1.0, 0.0, 0.0);
vec3 v_normal       : NORMAL = vec3(0.0, 1.0, 0.0);
vec4 v_position		: POSITION = vec4(0.0, 0.0, 0.0, 0.0);
mat3 v_tbnViewSpace : TEXCOORD1;
vec4 v_shadowcoord	: TEXCOORD2;


vec4 a_position     : POSITION;
vec3 a_normal       : NORMAL;
vec2 a_texcoord0    : TEXCOORD0;
vec3 a_tangent		: TANGENT;
vec3 a_bitangent 	: BINORMAL;
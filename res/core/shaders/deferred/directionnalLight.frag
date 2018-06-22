varying vec2 v_texcoord0;


uniform sampler2D gAlbedoSpec;
uniform sampler2D gNormal;
uniform sampler2D gDepth;


uniform mat4 u_inverseTransposeWorldViewMatrix;
uniform mat4 u_projectionMatrix;
uniform mat4 u_inverseProjectionMatrix;
uniform mat4 u_inverseViewMatrix;



// point light
uniform vec4 u_viewPos;
uniform vec4 u_lightPos;
uniform vec4 u_lightColor;
uniform vec4 u_lightRadius;

// directionnal light
uniform vec4 u_lightDirection;



#include "../common/common.sh"
#include "utils.glsl"


struct Light 
{
    vec3 position;
    vec3 color;
    float radius;
};

struct DirLight
{
    vec3 direction;
    vec3 color;  
};











vec2 poissonDisk[16] = vec2[]( 
   vec2( -0.94201624, -0.39906216 ), 
   vec2( 0.94558609, -0.76890725 ), 
   vec2( -0.094184101, -0.92938870 ), 
   vec2( 0.34495938, 0.29387760 ), 
   vec2( -0.91588581, 0.45771432 ), 
   vec2( -0.81544232, -0.87912464 ), 
   vec2( -0.38277543, 0.27676845 ), 
   vec2( 0.97484398, 0.75648379 ), 
   vec2( 0.44323325, -0.97511554 ), 
   vec2( 0.53742981, -0.47373420 ), 
   vec2( -0.26496911, -0.41893023 ), 
   vec2( 0.79197514, 0.19090188 ), 
   vec2( -0.24188840, 0.99706507 ), 
   vec2( -0.81409955, 0.91437590 ), 
   vec2( 0.19984126, 0.78641367 ), 
   vec2( 0.14383161, -0.14100790 ) 
);

float random(vec3 seed, int i){
    vec4 seed4 = vec4(seed,i);
    float dot_product = dot(seed4, vec4(12.9898,78.233,45.164,94.673));
    return fract(sin(dot_product) * 43758.5453);
}





uniform mat4 u_lightSpaceMatrix;
//uniform sampler2D s_shadowMap;
SAMPLER2DSHADOW(s_shadowMap, 3);
varying vec4 v_shadowcoord;
#if 0
float ShadowCalculation(vec4 fragPosLightSpace, float bias)
{   
    vec3 tex_coords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    tex_coords = tex_coords * 0.5 + 0.5;
    float depth = texture2D(s_shadowMap, tex_coords.xy).r;
    //float inShadow = (depth < tex_coords.z) ? 1.0 : 0.0;
    /*float currentDepth = tex_coords.z;
    float bias = 0.005;
    float inShadow = (currentDepth - bias > depth) ? 1.0 : 0.0;

    if(tex_coords.z > 1.0)
        inShadow = 0.0;*/

    //float bias = 0.005;
    float visibility = 1.0;
    /*if ( texture2D( s_shadowMap, tex_coords.xy ).r  <  tex_coords.z - bias)
    {
        visibility = 0.0;
    }*/

    for (int i=0;i<16;i++)
    {
       if ( texture2D( s_shadowMap, tex_coords.xy + poissonDisk[i]/400.0 ).r  <  tex_coords.z - bias)
        {
            visibility-=0.05;
        }
    }


    if(tex_coords.z > 1.0)
        visibility = 1.0;


    return visibility;
}
#endif


float NewShadow(vec4 fragPosLightSpace)
{
    vec3 projCoord = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoord = projCoord * 0.5 + 0.5;


    const float kTransparency = 0.0;

    vec3 shadowUV = projCoord; //projCoord.xyz / projCoord.w;
    float mapScale = 1.0 / 2048.0;

    shadowUV -= 0.0005; // bias

    float shadowColor = shadow2D(s_shadowMap, shadowUV);

    shadowColor += shadow2D(s_shadowMap, shadowUV.xyz + vec3( mapScale,  mapScale, 0));
    shadowColor += shadow2D(s_shadowMap, shadowUV.xyz + vec3( mapScale, -mapScale, 0));
    shadowColor += shadow2D(s_shadowMap, shadowUV.xyz + vec3( mapScale,     0, 0));
    shadowColor += shadow2D(s_shadowMap, shadowUV.xyz + vec3(-mapScale,  mapScale, 0));
    shadowColor += shadow2D(s_shadowMap, shadowUV.xyz + vec3(-mapScale, -mapScale, 0));
    shadowColor += shadow2D(s_shadowMap, shadowUV.xyz + vec3(-mapScale,     0, 0));
    shadowColor += shadow2D(s_shadowMap, shadowUV.xyz + vec3(        0,  mapScale, 0));
    shadowColor += shadow2D(s_shadowMap, shadowUV.xyz + vec3(        0, -mapScale, 0));

    shadowColor = shadowColor / 9.0;

    shadowColor += kTransparency;
    shadowColor = clamp(shadowColor, 0.0, 1.0);

    if(shadowUV.x >= 0.0 && shadowUV.y >= 0.0 && shadowUV.x <= 1.0 && shadowUV.y <= 1.0 )
    {
        return shadowColor;
    }
    else
    {
        return 1.0;
    }
    
}


#define Sampler sampler2DShadow
float hardShadow(Sampler _sampler, vec4 _shadowCoord, float _bias)
{
    vec3 texCoord = _shadowCoord.xyz/_shadowCoord.w;
    texCoord = texCoord * 0.5 + 0.5;
    return shadow2D(_sampler, vec3(texCoord.xy, texCoord.z-_bias) );
}


float PCF(Sampler _sampler, vec4 _shadowCoord, float _bias, vec2 _texelSize)
{
    vec3 texCoord = _shadowCoord.xyz/_shadowCoord.w;
    texCoord = texCoord * 0.5 + 0.5;

    bool outside = any(greaterThan(texCoord, vec3_splat(1.0)))
                || any(lessThan   (texCoord, vec3_splat(0.0)))
                 ;

    if (outside)
    {
        return 1.0;
    }

    float result = 0.0;
    vec2 offset = _texelSize * _shadowCoord.w;

    result += hardShadow(_sampler, _shadowCoord + vec4(vec2(-1.5, -1.5) * offset, 0.0, 0.0), _bias);
    result += hardShadow(_sampler, _shadowCoord + vec4(vec2(-1.5, -0.5) * offset, 0.0, 0.0), _bias);
    result += hardShadow(_sampler, _shadowCoord + vec4(vec2(-1.5,  0.5) * offset, 0.0, 0.0), _bias);
    result += hardShadow(_sampler, _shadowCoord + vec4(vec2(-1.5,  1.5) * offset, 0.0, 0.0), _bias);

    result += hardShadow(_sampler, _shadowCoord + vec4(vec2(-0.5, -1.5) * offset, 0.0, 0.0), _bias);
    result += hardShadow(_sampler, _shadowCoord + vec4(vec2(-0.5, -0.5) * offset, 0.0, 0.0), _bias);
    result += hardShadow(_sampler, _shadowCoord + vec4(vec2(-0.5,  0.5) * offset, 0.0, 0.0), _bias);
    result += hardShadow(_sampler, _shadowCoord + vec4(vec2(-0.5,  1.5) * offset, 0.0, 0.0), _bias);

    result += hardShadow(_sampler, _shadowCoord + vec4(vec2(0.5, -1.5) * offset, 0.0, 0.0), _bias);
    result += hardShadow(_sampler, _shadowCoord + vec4(vec2(0.5, -0.5) * offset, 0.0, 0.0), _bias);
    result += hardShadow(_sampler, _shadowCoord + vec4(vec2(0.5,  0.5) * offset, 0.0, 0.0), _bias);
    result += hardShadow(_sampler, _shadowCoord + vec4(vec2(0.5,  1.5) * offset, 0.0, 0.0), _bias);

    result += hardShadow(_sampler, _shadowCoord + vec4(vec2(1.5, -1.5) * offset, 0.0, 0.0), _bias);
    result += hardShadow(_sampler, _shadowCoord + vec4(vec2(1.5, -0.5) * offset, 0.0, 0.0), _bias);
    result += hardShadow(_sampler, _shadowCoord + vec4(vec2(1.5,  0.5) * offset, 0.0, 0.0), _bias);
    result += hardShadow(_sampler, _shadowCoord + vec4(vec2(1.5,  1.5) * offset, 0.0, 0.0), _bias);

    return result / 16.0;
}





void main()
{
    // retrieve data from gbuffer    
    vec3 Diffuse = texture2D(gAlbedoSpec, v_texcoord0).rgb;
    float Specular = texture2D(gAlbedoSpec, v_texcoord0).a;
    float Depth = texture2D(gDepth, v_texcoord0).r;
    vec3 Normal = texture2D(gNormal, v_texcoord0).rgb;

    
    // get world pos from depth buffer
    vec3 fragPos = worldPosFromDepth(Depth);    

    DirLight light;
    light.direction = u_lightDirection.xyz;
    light.color = u_lightColor.rgb;

    vec3 lighting = vec3(0.0);  //  = Diffuse * 0.0; // hard-coded ambient component
    vec3 viewDir  = normalize(u_viewPos.xyz - fragPos);    

    // diffuse
    vec3 lightDir = normalize(-light.direction);
    vec3 diffuse = max(dot(Normal, lightDir), 0.0) * Diffuse * light.color;

    // specular
    vec3 reflectDir = reflect(-lightDir, Normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 2.0);
    vec3 specular = light.color * spec * Specular;
      





    // shadow begin

    //float shadow = 1.0 - ShadowCalculation(v_shadowcoord);
    //float shadow = texture2D(s_shadowMap, v_shadowcoord.xy).r;
    //float shadow = texture2D(s_shadowMap, v_texcoord0).r;
    //float shadow = 1.0 - ShadowCalculation(v_texcoord0);

    

    float cosTheta = clamp(dot(Normal, lightDir), 0.0, 1.0);
    float bias = 0.005 * tan(acos(cosTheta)); // cosTheta is dot( n,l ), clamped between 0 and 1
    bias = clamp(bias, 0, 0.1);

    //float bias = 0.005;



   /* vec4 fragPosLightSpace = u_lightSpaceMatrix * vec4(fragPos, 1.0);
    float shadow = ShadowCalculation(fragPosLightSpace, bias);
    diffuse *= shadow;
    specular *= shadow;*/

    // normal offset
    //vec3 normalOffsetScale = vec3(0.05);
    float cosLight = dot(Normal, lightDir);
    float slopeScale = clamp(1.0 - cosLight, 0.0, 1.0);
    float normalOffsetScale =  0.05 * slopeScale;
    vec4 shadowOffset = u_lightSpaceMatrix * vec4(fragPos + Normal * normalOffsetScale, 1.0);

    // pcf 9x
    vec4 fragPosLightSpace = u_lightSpaceMatrix * vec4(fragPos, 1.0);
    float shadow = NewShadow(shadowOffset);  //fragPosLightSpace);
    diffuse *= shadow;
    specular *= shadow;


    // bgfx pcf
    /*vec2 texelSize = vec2_splat(1.0/512.0);
    vec4 fragPosLightSpace = u_lightSpaceMatrix * vec4(fragPos + Normal * normalOffsetScale, 1.0);
    //float visibility = PCF(s_shadowMap, fragPosLightSpace, bias, texelSize);
    float visibility = PCF(s_shadowMap, fragPosLightSpace, 0.000, texelSize);
    diffuse *= visibility;
    specular *= visibility;*/


    // -- end shadow


    // result
    lighting += diffuse + specular;

    vec4 lightResult = vec4(lighting, 1.0);
    gl_FragData[0] = lightResult;




    // write to bright buffer

    vec4 brightResult;

    // check whether fragment output is higher than threshold, if so output as brightness color
    float brightness = dot(lightResult.rgb, vec3(0.2126, 0.7152, 0.0722));
    if(brightness > 1.0)
        brightResult = vec4(lightResult.rgb, 1.0);
    else
        brightResult = vec4(0.0, 0.0, 0.0, 1.0);

    gl_FragData[1] = brightResult;




}

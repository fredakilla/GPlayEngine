$input v_texcoord0, v_color, v_normal, v_position, v_tbnViewSpace, v_shadowcoord

#include "../common/common.sh"


//#define DIRECTIONAL_LIGHT_COUNT 1
//#define POINT_LIGHT_COUNT 1
//#define SPOT_LIGHT_COUNT 1

#define DIFFUSE_MAP
#define BUMP_MAP
#define SPECULAR_MAP
#define SPECULAR
//#define TEXTURE_DISCARD_ALPHA
//#define FOG
#define SHADOW



#ifndef DIRECTIONAL_LIGHT_COUNT
#define DIRECTIONAL_LIGHT_COUNT 0
#endif
#ifndef SPOT_LIGHT_COUNT
#define SPOT_LIGHT_COUNT 0
#endif
#ifndef POINT_LIGHT_COUNT
#define POINT_LIGHT_COUNT 0
#endif
#if (DIRECTIONAL_LIGHT_COUNT > 0) || (POINT_LIGHT_COUNT > 0) || (SPOT_LIGHT_COUNT > 0)
#define LIGHTING
#endif


SAMPLER2D(u_diffuseTexture, 0);
SAMPLER2D(u_normalTexture, 1);
SAMPLER2D(u_specularTexture, 2);
SAMPLER2D(s_shadowMap, 3);

uniform vec4 u_ambientColor;


#if (DIRECTIONAL_LIGHT_COUNT > 0)
uniform vec4 u_directionalLightDirection[DIRECTIONAL_LIGHT_COUNT];
uniform vec4 u_directionalLightColor[DIRECTIONAL_LIGHT_COUNT];
#endif

#if (POINT_LIGHT_COUNT > 0)
uniform vec4 u_pointLightPosition[POINT_LIGHT_COUNT];
uniform vec4 u_pointLightColor[POINT_LIGHT_COUNT];
uniform vec4 u_pointLightAttenuation[POINT_LIGHT_COUNT];
#endif

#if (SPOT_LIGHT_COUNT > 0)
uniform vec4 u_spotLightPosition[SPOT_LIGHT_COUNT];
uniform vec4 u_spotLightDirection[SPOT_LIGHT_COUNT];
uniform vec4 u_spotLightColor[SPOT_LIGHT_COUNT];
uniform vec4 u_spotLightAttenuation[SPOT_LIGHT_COUNT];
#endif



struct Material
{
    vec3 diffuse;
    vec3 specular;
    float shininess;
};

struct DirLight
{
    vec3 direction;    
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct PointLight
{
    vec3 position; 
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;       
    float constant;
    float linear;
    float quadratic;   
};

struct SpotLight
{
    vec3 position;
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float constant;
    float linear;
    float quadratic;
    float cutOff;
    float outerCutOff;
};

Material material;



#if (FOG)

uniform vec3 u_fogColor;
uniform vec2 u_fogDistance; // x=near, y=far

struct FogInfo
{
    float maxDist;
    float minDist;
    vec3 color;
};

FogInfo fog;

#endif


float ShadowCalculation(vec4 fragPosLightSpace)
{   
    vec3 tex_coords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    tex_coords = tex_coords * 0.5 + 0.5;
    float depth = texture2D(s_shadowMap, tex_coords.xy).r;
    //float inShadow = (depth < tex_coords.z) ? 1.0 : 0.0;
    float currentDepth = tex_coords.z;
    float bias = 0.005;
    float inShadow = (currentDepth - bias > depth) ? 1.0 : 0.0;

    return inShadow;
}

float computeSpecular(vec3 lightDir, vec3 normal, vec3 viewDir)
{
    // Phong
    //vec3 reflectDir = reflect(-lightDir, normal);
    //float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);

    // Blinn
    vec3 halfwayDir = normalize(lightDir + viewDir);  
    float spec = pow(max(dot(normal, halfwayDir), 0.0), material.shininess);

    return spec;
}

// calculates the color when using a directional light.
vec3 computeDirLight(DirLight light, vec3 normal, vec3 viewDir)
{
    vec3 lightDir = normalize(-light.direction);

    // diffuse
    float diff = max(dot(normal, lightDir), 0.0);

    // specular
#if defined(SPECULAR)
    float spec = computeSpecular(lightDir, normal, viewDir);
#endif

#if defined(SHADOW) 
    float shadow = 1.0 - ShadowCalculation(v_shadowcoord);
    diff *= shadow;
    spec *= shadow;
#endif

    // combine results

#if defined(SPECULAR) 
    vec3 ambient = light.ambient * material.diffuse;
    vec3 diffuse = light.diffuse * diff * material.diffuse;
    vec3 specular = light.specular * spec * material.specular;
    return (ambient + diffuse + specular);
#else
    vec3 ambient = light.ambient * material.diffuse;
    vec3 diffuse = light.diffuse * diff * material.diffuse;
    return (ambient + diffuse);
#endif  
}



// calculates the color when using a point light.
vec3 computePointLight(PointLight light, vec3 normal, vec3 pos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - pos);

    // diffuse
    float diff = max(dot(normal, lightDir), 0.0);

    // specular
#if defined(SPECULAR)
    float spec = computeSpecular(lightDir, normal, viewDir);
#endif

    // attenuation
    float distance = length(light.position - pos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

    // combine results

#if defined(SPECULAR)    
    vec3 ambient = light.ambient *  material.diffuse;
    vec3 diffuse = light.diffuse * diff *  material.diffuse;
    vec3 specular = light.specular * spec * material.specular;
    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;
    return (ambient + diffuse + specular);
#else
    vec3 ambient = light.ambient *  material.diffuse;
    vec3 diffuse = light.diffuse * diff *  material.diffuse;
    ambient *= attenuation;
    diffuse *= attenuation;
    return (ambient + diffuse);
#endif
}

// calculates the color when using a spot light.
vec3 computeSpotLight(SpotLight light, vec3 normal, vec3 pos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - pos);

    // diffuse
    float diff = max(dot(normal, lightDir), 0.0);

    // specular
#if defined(SPECULAR)  
    float spec = computeSpecular(lightDir, normal, viewDir);
#endif

    // attenuation
    float distance = length(light.position - pos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));  

    // spotlight intensity
    float theta = dot(lightDir, normalize(-light.direction)); 
    float epsilon = light.cutOff - light.outerCutOff;
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);

    // combine results

#if defined(SPECULAR)
    vec3 ambient = light.ambient * material.diffuse;
    vec3 diffuse = light.diffuse * diff * material.diffuse;
    vec3 specular = light.specular * spec * material.specular;
    ambient *= attenuation * intensity;
    diffuse *= attenuation * intensity;
    specular *= attenuation * intensity;
    return (ambient + diffuse + specular);
#else
    vec3 ambient = light.ambient * material.diffuse;
    vec3 diffuse = light.diffuse * diff * material.diffuse;
    ambient *= attenuation * intensity;
    diffuse *= attenuation * intensity;
    return (ambient + diffuse);
#endif
}




void main()
{
    vec3 normalVector = v_normal.xyz; 
    vec3 position = v_position.xyz;   

#if defined(DIFFUSE_MAP)
    vec4 diffuseMap = texture2D(u_diffuseTexture, v_texcoord0);
#else
    vec4 diffuseMap = vec4(0.5, 0.5, 0.5, 1.0);
#endif


#if defined(TEXTURE_DISCARD_ALPHA)
    if (diffuseMap.a < 0.5)
        discard;
#endif


#if defined(BUMP_MAP)
    vec4 normalMap = texture2D(u_normalTexture, v_texcoord0) * 2 - 1;
    normalVector = v_tbnViewSpace * normalMap.rgb;
#endif

    normalVector = normalize(normalVector);


#if defined(SPECULAR)
#if defined(SPECULAR_MAP)
    vec4 specularMap = texture2D(u_specularTexture, v_texcoord0);
#else
    vec4 specularMap = vec4(0.5, 0.5, 0.5, 1.0);
#endif
#endif



    // setup material
    material.diffuse = diffuseMap.rgb;
#if defined(SPECULAR)
    material.specular = specularMap.rgb;
    material.shininess = 16.0;
#endif



#if (DIRECTIONAL_LIGHT_COUNT > 0)
    DirLight dirLight[1];
    dirLight[0].direction = u_directionalLightDirection[0].xyz;
    dirLight[0].ambient = u_ambientColor.rgb;
    dirLight[0].diffuse = u_directionalLightColor[0].rgb;
    dirLight[0].specular = vec3(1.0, 1.0, 1.0);
#endif

#if (POINT_LIGHT_COUNT > 0)
    PointLight pointLight[1];
    pointLight[0].position =  u_pointLightPosition[0].xyz;
    pointLight[0].ambient = u_ambientColor.rgb;
    pointLight[0].diffuse = u_pointLightColor[0].rgb;
    pointLight[0].specular = vec3(1.0, 1.0, 1.0);
    pointLight[0].constant = u_pointLightAttenuation[0].x;
    pointLight[0].linear = u_pointLightAttenuation[0].y;
    pointLight[0].quadratic = u_pointLightAttenuation[0].z;
#endif

#if (SPOT_LIGHT_COUNT > 0)
    SpotLight spotLight[1];
    spotLight[0].position = u_spotLightPosition[0].xyz;
    spotLight[0].direction =  u_spotLightDirection[0].xyz;
    spotLight[0].ambient = u_ambientColor.rgb;
    spotLight[0].diffuse = u_spotLightColor[0].rgb;
    spotLight[0].specular = vec3(1.0, 1.0, 1.0);
    spotLight[0].constant = u_spotLightAttenuation[0].x;
    spotLight[0].linear = u_spotLightAttenuation[0].y;
    spotLight[0].quadratic = u_spotLightAttenuation[0].z;
    spotLight[0].cutOff = 0.6;
    spotLight[0].outerCutOff = 0.5;
#endif



    vec3 viewDir = normalize(-position);
    vec3 result = vec3(0,0,0);


    // directional lights
#if (DIRECTIONAL_LIGHT_COUNT > 0)
    for(int i = 0; i < DIRECTIONAL_LIGHT_COUNT; i++)
    {
        result = computeDirLight(dirLight[i], normalVector, viewDir);
    }
#endif

    // point lights
#if (POINT_LIGHT_COUNT > 0)
    for(int i = 0; i < POINT_LIGHT_COUNT; i++)
    {
        result += computePointLight(pointLight[i], normalVector, position, viewDir);   
    }
#endif

    // spot lights
#if (SPOT_LIGHT_COUNT > 0)
    for(int i = 0; i < SPOT_LIGHT_COUNT; i++)
    {
        result += computeSpotLight(spotLight[i], normalVector, position, viewDir);  
    }
#endif



#if defined(FOG)
    fog.maxDist = 160;
    fog.minDist = 50;
    fog.color = vec3(0.5, 0.5, 0.5);

    float dist = abs(position.z);
    float fogFactor = (fog.maxDist - dist) / (fog.maxDist -  fog.minDist);
    fogFactor = clamp(fogFactor, 0.0, 1.0);
    result = mix(fog.color, result, fogFactor);
#endif

    // gamma    
    //result = pow(result, vec3(1.0/2.2));

    gl_FragColor = vec4(result, 1.0);
}

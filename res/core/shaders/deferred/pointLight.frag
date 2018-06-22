varying vec2 v_texcoord0;


uniform sampler2D gNormal;
uniform sampler2D gAlbedoSpec;
uniform sampler2D gDepth;

uniform mat4 u_inverseTransposeWorldViewMatrix;
uniform mat4 u_projectionMatrix;
uniform mat4 u_inverseProjectionMatrix;
uniform mat4 u_inverseViewMatrix;

uniform vec4 u_viewPos;
uniform vec4 u_lightPos;
uniform vec4 u_lightColor;
uniform vec4 u_lightRadius;

#include "utils.glsl"


struct Light 
{
    vec3 position;
    vec3 color;
    float radius;
};








void main()
{
    // retrieve data from gbuffer
    vec3 Normal = texture2D(gNormal, v_texcoord0).rgb;
    vec3 Diffuse = texture2D(gAlbedoSpec, v_texcoord0).rgb;
    float Specular = texture2D(gAlbedoSpec, v_texcoord0).a;
    float Depth = texture2D(gDepth, v_texcoord0).r;
    
    // get world pos from depth buffer
    vec3 fragPos = worldPosFromDepth(Depth);    

    Light light;
    light.position = u_lightPos.xyz;
    light.color = u_lightColor.rgb;
    light.radius = u_lightRadius.x;

    vec3 lighting = vec3(0.0);  //  = Diffuse * 0.0; // hard-coded ambient component
    vec3 viewDir  = normalize(u_viewPos.xyz - fragPos);    

    // diffuse
    vec3 lightDir = normalize(light.position - fragPos);
    vec3 diffuse = max(dot(Normal, lightDir), 0.0) * Diffuse * light.color;

    // specular
    vec3 halfwayDir = normalize(lightDir + viewDir);  
    float spec = pow(max(dot(Normal, halfwayDir), 0.0), 16.0);
    vec3 specular = light.color * spec * Specular;


    // attenuation
        //float distance = length(light.position - FragPos);
        //float attenuation = 1.0 / (1.0 + light.Linear * distance + light.Quadratic * distance * distance);
        
        //float attenuation = clamp(1.0 - dot(lightDir, lightDir), 0.0, 1.0);
        
        //vec3 distance = (light.Position - FragPos) / u_lightRadius.x;
        //float attenuation = 1.0f - dot(distance, distance);

    float attenuation = smoothstep(light.radius, 0, length(light.position - fragPos));

      

    // result
    diffuse *= attenuation;
    specular *= attenuation;
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

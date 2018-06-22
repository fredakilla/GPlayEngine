float LinearizeDepth(float depth, float zNear, float zFar)
{    
    return (2.0 * zNear) / (zFar + zNear - depth * (zFar - zNear));
}

vec3 worldPosFromDepth(float depth)
{
    float z = depth * 2.0 - 1.0;

    // get clip space
    vec4 clipSpacePosition = vec4(v_texcoord0.xy * 2.0 - 1.0, z, 1.0);

    // clip space -> view space
    vec4 viewSpacePosition = u_inverseProjectionMatrix * clipSpacePosition;

    // perspective division
    viewSpacePosition /= viewSpacePosition.w;

    // view space -> world space
    vec4 worldSpacePosition = u_inverseViewMatrix * viewSpacePosition;

    return worldSpacePosition.xyz;
}

varying vec2 v_texcoord0;

uniform sampler2D image;
uniform vec4 u_direction;

#include "../utils/blur.sh"

void main()
{
   float size = 512; //2048.0 / 10.0;
   vec2 texelSize = vec2(size, size);
   gl_FragColor = blur9(image, v_texcoord0, texelSize, u_direction.xy);
}

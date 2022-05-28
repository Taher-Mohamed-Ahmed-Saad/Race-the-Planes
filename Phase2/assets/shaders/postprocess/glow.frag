#version 330 core
out vec4 FragColor;
  
in vec2 tex_coord;

uniform sampler2D tex;
  
uniform float weight[5] = float[] (0.227027, 0.1945946, 0.1216216, 0.054054, 0.016216);
uniform int size=5;
vec3 threshold(vec3 vector,float thresh){
    if(length(vector)>thresh)
        return vector;
    return vec3(0,0,0);
}
void main()
{             
    vec2 tex_offset = 1.0 / textureSize(tex, 0); // gets size of single texel
    vec3 result = texture(tex, tex_coord).rgb; // current fragment's contribution
    float thresh=1.6;
    
    for(int i = 0; i < size; ++i)
    {
        vec3 right=texture(tex, tex_coord + vec2(tex_offset.x * i, 0.0)).rgb;
        vec3 left=texture(tex, tex_coord - vec2(tex_offset.x * i, 0.0)).rgb;

        vec3 top=texture(tex, tex_coord + vec2(tex_offset.y * i, 0.0)).rgb;
        vec3 bottom=texture(tex, tex_coord - vec2(tex_offset.y * i, 0.0)).rgb;
    
        result += (threshold(right,thresh)+threshold(right,thresh)) * weight[i];
        result += (threshold(top,thresh)+threshold(bottom,thresh)) * weight[i];
    }

    FragColor = vec4(result, 1.0);
}
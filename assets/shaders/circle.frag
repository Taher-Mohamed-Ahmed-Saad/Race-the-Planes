#version 330 core

out vec4 frag_color;

uniform float radius;
uniform vec2 center;

uniform vec4 inside_color = vec4(1.0, 0.0, 0.0, 1.0);
uniform vec4 outside_color = vec4(0.0, 0.0, 0.0, 1.0);

void main(){
    //This condition is to specify the the bound of the circle acording to eq: (x-xc)² + (y-yc)² = r²
    //x: gl_FragCoord.x , xc: center[0]
    //y: gl_FragCoord.y , yc: center[1]
    if ( (gl_FragCoord.x -center[0]) * (gl_FragCoord.x -center[0]) + (gl_FragCoord.y -center[1]) * (gl_FragCoord.y -center[1]) <= radius * radius){
        frag_color = inside_color;
    } else {
        frag_color = outside_color;
    }
}
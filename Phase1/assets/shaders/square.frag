#version 330 core

out vec4 frag_color;

//TODO: Define uniforms for the center and the side-length
uniform vec2 center;
uniform float side_length;

uniform vec4 inside_color = vec4(1.0, 0.0, 0.0, 1.0);
uniform vec4 outside_color = vec4(0.0, 0.0, 0.0, 1.0);

void main(){
    //TODO: Write code that will draw the square
    //This condition is to specify the the bound of the square acording to eq: max[ |x-xc| , |y-yc| ] <= side_length/2
    //x: gl_FragCoord.x , xc: center[0]
    //y: gl_FragCoord.y , yc: center[1]
    if (2*max(abs(center[0] - gl_FragCoord.x) , abs(center[1] - gl_FragCoord.y)) <= side_length){
        frag_color = inside_color;
    } else {
        frag_color = outside_color;
    }
}
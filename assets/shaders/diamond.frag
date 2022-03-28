#version 330 core

out vec4 frag_color;

uniform vec2 center;
uniform float side_length;

uniform vec4 inside_color = vec4(1.0, 0.0, 0.0, 1.0);
uniform vec4 outside_color = vec4(0.0, 0.0, 0.0, 1.0);

void main(){
    //This condition is to specify the the bound of the diamond acording to eq: |x-xc| + |y-yc|  <= side_length
    //x: gl_FragCoord.x , xc: center[0]
    //y: gl_FragCoord.y , yc: center[1]
    if( abs( (gl_FragCoord.x -center[0]) ) + abs( (gl_FragCoord.y -center[1]) ) <= side_length/2){
        frag_color = inside_color;
    } else {
        frag_color = outside_color;
    }
}
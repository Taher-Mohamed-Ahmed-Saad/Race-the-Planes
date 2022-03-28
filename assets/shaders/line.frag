#version 330 core

out vec4 frag_color;

uniform float slope;
uniform float intercept;

uniform vec4 inside_color = vec4(1.0, 0.0, 0.0, 1.0);
uniform vec4 outside_color = vec4(0.0, 0.0, 0.0, 1.0);

void main(){
    //This condition is to specify the the bound of the line acording to eq: y ≤ ax + b
    //x: gl_FragCoord.x
    //y: gl_FragCoord.y
    if(gl_FragCoord.y <= slope *gl_FragCoord.x + intercept){
        frag_color = inside_color;
    } else {
        frag_color = outside_color;
    }
}
#version 330 core

out vec4 frag_color;

//TODO: Define uniforms for the center and the radius
uniform vec2 center;
uniform float raduis;

uniform vec4 inside_color = vec4(1.0, 0.0, 0.0, 1.0);
uniform vec4 outside_color = vec4(0.0, 0.0, 0.0, 1.0);

void main(){
    //TODO: Write code that will draw the circle
    /*
        gl_FragCoord — contains the window-relative coordinates of the current fragment
        https://www.khronos.org/registry/OpenGL-Refpages/gl4/html/gl_FragCoord.xhtml

    */    
    if ((gl_FragCoord.x -center[0] )^2 + (gl_FragCoord.y -center[1])^2 <= raduis^2){
        frag_color = inside_color;
    } else {
        frag_color = outside_color;
    }
}
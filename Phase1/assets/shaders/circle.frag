#version 330 core

out vec4 frag_color;

//TODO: Define uniforms for the center and the radius
uniform vec2 center;
uniform float radius;

uniform vec4 inside_color = vec4(1.0, 0.0, 0.0, 1.0);
uniform vec4 outside_color = vec4(0.0, 0.0, 0.0, 1.0);

void main(){
    //TODO: Write code that will draw the circle
    /*
        gl_FragCoord — contains the window-relative coordinates of the current fragment
        https://www.khronos.org/registry/OpenGL-Refpages/gl4/html/gl_FragCoord.xhtml

    */  
    //This condition is to specify the the bound of the circle acording to eq: (x-xc)² + (y-yc)² = r²
    //x: gl_FragCoord.x , xc: center[0]
    //y: gl_FragCoord.y , yc: center[1]  
    if (((gl_FragCoord.x -center[0] ) * (gl_FragCoord.x -center[0] )+ (gl_FragCoord.y -center[1])*(gl_FragCoord.y -center[1])) <= radius * radius ){
        frag_color = inside_color;
    } else {
        frag_color = outside_color;
    }
}
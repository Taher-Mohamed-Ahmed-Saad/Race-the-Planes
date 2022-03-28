#pragma once

#include <application.hpp>
#include <shader/shader.hpp>

// This function allows us to read glm vectors from json 
namespace glm {
    template<length_t L, typename T, qualifier Q>
    void from_json(const nlohmann::json& j, vec<L, T, Q>& v){
        for(length_t index = 0; index < L; ++index)
            v[index] = j[index].get<T>();
    }
}

class FullscreenTriangleState: public our::State {
    
    our::ShaderProgram program;

    GLuint VAO;

    // onInitialize() function is called once before the state starts
    void onInitialize() override {
        
        // Here we get the json of the scene configuration
        const auto& config = getApp()->getConfig()["scene"];

        // Then we get the path for vertex and fragment shader
        // We used "assets/shaders/fullscreen.vert" and "assets/shaders/circle.frag" as the defaults if the paths are not defined
        std::string vertex_shader_path = config.value("vertex-shader", "assets/shaders/fullscreen.vert");
        std::string fragment_shader_path = config.value("fragment-shader", "assets/shaders/circle.frag");

        // Here we create a shader, attach the vertex and fragment shader to it then link
        program.create();
        program.attach(vertex_shader_path, GL_VERTEX_SHADER);
        program.attach(fragment_shader_path, GL_FRAGMENT_SHADER);
        program.link();

        // We call use() since we will send uniforms to this program
        program.use();
        // We loop over every uniform in the configuration and send to the program
        if(const auto& uniforms = config["uniforms"]; uniforms.is_object()){
            for(auto& [name, uniform] : uniforms.items()){
                std::string type = uniform.value("type", "");
                if(type == "float"){
                    float value = uniform.value("value", 0.0f);
                    program.set(name, value);
                } else if(type == "vec2"){
                    glm::vec2 value = uniform.value("value", glm::vec2(0,0));
                    program.set(name, value);
                } else if(type == "vec4"){
                    glm::vec4 value = uniform.value("value", glm::vec4(0,0,0,0));
                    program.set(name, value);
                }
            }
        }

        //Generate vertex array object
        //Operand1: specify number of vertex array objects to generate
        //Operand2: This should be an array that will hold the genrated vertex array object names
        //          But we specified only one UINT because we created only one vertex array object
        glGenVertexArrays(1, &VAO);
        //binding vertex array 
        glBindVertexArray(VAO);

        // We set the clear color to be black
        glClearColor(0.0, 0.0, 0.0, 1.0);
    }

    // onDraw(deltaTime) function is called every frame 
    void onDraw(double deltaTime) override {
        //At the start of frame we want to clear the screen. Otherwise we would still see the results from the previous frame.
        glClear(GL_COLOR_BUFFER_BIT);
        
        //drawing triangles using vertex array object and program
        //Operand1: Mode the openGL will use when drawing
        //Operand2: Specifies the starting index form which drawing begins
        //Operand3: Specifies number of vertices that will be drawn 
        glDrawArrays(GL_TRIANGLES, 0 , 3);

    }

    // onInitialize() function is called once after the state ends
    void onDestroy() override {
        //Delete the vertex Array
        //Operand1: number of vertex array objects to be deleted
        //Operand2: should be an array carring IDs of the vertex array objects
        glDeleteVertexArrays(1 , &VAO);
    }
};
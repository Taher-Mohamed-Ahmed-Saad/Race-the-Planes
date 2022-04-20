#ifndef SHADER_HPP
#define SHADER_HPP

#include <string>
#include <iostream>

#include <glad/gl.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace our {

    class ShaderProgram {

    private:
        //Shader Program Handle
        GLuint program;

    public:
        void create();
        void destroy();

        ShaderProgram(){ program = 0; }
        ~ShaderProgram(){ destroy(); }

        bool attach(const std::string &filename, GLenum type) const;

        bool link() const;

        void use() { 
            //TODO: call opengl to use the program identified by this->program
            //This so to specify the program the OpenGL will use in the current rendering state
            //Specified program will have our shader attached to it for the rendering process
            //Operand 1: program object that is declared in this object from the class
            glUseProgram(this->program);
        }

        GLuint getUniformLocation(const std::string &name) {
            //TODO: call opengl to get the uniform location for the uniform defined by name from this->program
            //Returns the location of a uniform variable (returns -1 if no active uniform variable with this name is found)
            //operand1: program object that is declared in this object from the class
            //operand2: name of the uniform variable whose location is to be queried.
            return glGetUniformLocation(this->program,name.c_str());  
        }

        void set(const std::string &uniform, GLfloat value) {
            //TODO: call opengl to set the value to the uniform defined by name
            // glUniform1f(getUniformLocation(uniform.c_str()) , value);

            //Getting location of the uniform variable we want to set it's value
            GLuint location = getUniformLocation(uniform);
            //Now checking if the provided name is actually for an active variable
            if(location != -1)
                //If yes the use the location returned previously to set it's value with required value
                glUniform1f(location, value);
            else
                //If not the print an error
                std::cerr<<"No active uniform variable with the provided name"<<std::endl;
        }

        void set(const std::string &uniform, glm::vec2 value) {
            //TODO: call opengl to set the value to the uniform defined by name
            // glUniform2f(getUniformLocation(uniform.c_str()),value[0] ,value[1]);


            //Getting location of the uniform variable we want to set it's value
            GLuint location = getUniformLocation(uniform);
            //Now checking if the provided name is actually for an active variable
            if(location != -1)
                //If yes the use the location returned previously to set it's value with required value
                glUniform2f(location, value[0], value[1]);
            else
                //If not the print an error
                std::cerr<<"No active uniform vec2 with the provided name"<<std::endl;
        }

        void set(const std::string &uniform, glm::vec3 value) {
            //TODO: call opengl to set the value to the uniform defined by name
            // glUniform3f(getUniformLocation(uniform.c_str()),value[0] ,value[1],value[2]);


            //Getting location of the uniform variable we want to set it's value
            GLuint location = getUniformLocation(uniform);

            //Now checking if the provided name is actually for an active variable
            if(location != -1)
                //If yes the use the location returned previously to set it's value with required value
                glUniform3f(location, value[0], value[1], value[2]);
            else
                //If not the print an error
                std::cerr<<"No active uniform vec3 with the provided name"<<std::endl;
        }

        void set(const std::string &uniform, glm::vec4 value) {
            //TODO: call opengl to set the value to the uniform defined by name
            // glUniform4f(getUniformLocation(uniform.c_str()),value[0] ,value[1],value[2],value[3]);


            //Getting location of the uniform variable we want to set it's value
            GLuint location = getUniformLocation(uniform);

            //Now checking if the provided name is actually for an active variable
            if(location != -1)
                //If yes the use the location returned previously to set it's value with required value
                glUniform4f(location, value[0], value[1], value[2], value[3]);
            else
                //If not the print an error
                std::cerr<<"No active uniform vec4 with the provided name"<<std::endl;
        }


        //TODO: Delete the copy constructor and assignment operator
        //Question: Why do we do this? Hint: Look at the deconstructor
        /* 
            to avoid having 2 pointers to the same object so when object is destroyed 
            the second will still point to it so it will delete wrong data from the memory 
            as it garbage
        */
        ShaderProgram (const ShaderProgram&) = delete;
        ShaderProgram& operator= (const ShaderProgram&) = delete;
    };

}

#endif
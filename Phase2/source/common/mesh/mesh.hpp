#pragma once

#include <glad/gl.h>
#include "vertex.hpp"

#include<iostream>
namespace our {

    #define ATTRIB_LOC_POSITION 0
    #define ATTRIB_LOC_COLOR    1
    #define ATTRIB_LOC_TEXCOORD 2
    #define ATTRIB_LOC_NORMAL   3

    class Mesh {
        // Here, we store the object names of the 3 main components of a mesh:
        // A vertex array object, A vertex buffer and an element buffer
        unsigned int VBO, EBO;
        unsigned int VAO;
        // We need to remember the number of elements that will be draw by glDrawElements 
        GLsizei elementCount;
    public:

        // The constructor takes two vectors:
        // - vertices which contain the vertex data.
        // - elements which contain the indices of the vertices out of which each rectangle will be constructed.
        // The mesh class does not keep a these data on the RAM. Instead, it should create
        // a vertex buffer to store the vertex data on the VRAM,
        // an element buffer to store the element data on the VRAM,
        // a vertex array object to define how to read the vertex & element buffer during rendering 
        Mesh(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& elements)
        {
            
            //TODO: (Req 1) Write this function

            // remember to store the number of elements in "elementCount" since you will need it for drawing
            // For the attribute locations, use the constants defined above: ATTRIB_LOC_POSITION, ATTRIB_LOC_COLOR, etc
            /*
                Binding Vertex Array so that we don't need to copy the data each time we need to render this object
                it will contain all the Data that we copy
            */
            glGenVertexArrays(1,&VAO);
            glBindVertexArray(VAO); 

            
            // in order to render the mesh we need to copy the data to the GPU memory using Buffers
            
            // this will Create 1 new buffer
            glGenBuffers(1,&VBO);
            // this will bind the new buffer to OpenGl array buffer  
            glBindBuffer(GL_ARRAY_BUFFER,VBO);
            /*
            this will copy the data the GL_ARRAY_BUFFER
            parameters:
                1- the buffer that we will copy to (GL_ARRAY_BUFFER)
                2- size of the data that we need to copy = number of vetices* size of a single vertex
                3- pointer to the actual data (not the vector)
                3- usage: since the mesh will be copied once and used many times we will use GL_STATIC_DRAW
            */
            glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * vertices.size(), vertices.data(), GL_STATIC_DRAW);


            /**
             * now the data is in the GPU memory but the shaders don't actually use it
             * each vertex looks like
             * 1- (3 * float)  -> position
             * 2- (4 * byte)   -> color
             * 3- (2 * float)  -> texture coordinates
             * 4- (3 * float)  -> normal vector 
             */

            // the stride is the same for all of them = Size of singe vertex
            // the position is the first thing in the vertex so it has offset = 0
            // this will enable let the shader know that every first 3 floats in the vertex in the buffer are the position
            glVertexAttribPointer(
                ATTRIB_LOC_POSITION, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)0);
            // this will enable the vector at location 0 (position)
            glEnableVertexAttribArray(ATTRIB_LOC_POSITION);

            // similar to the position however we sent the color as 4 bytes each 0->255 
            // however the fragment shader needs them to be 0->1
            // thats why we set normalized to true
            glVertexAttribPointer(ATTRIB_LOC_COLOR, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(Vertex), (void *)(sizeof(Vertex::position)));
            glEnableVertexAttribArray(ATTRIB_LOC_COLOR);

            glVertexAttribPointer(ATTRIB_LOC_TEXCOORD, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)(sizeof(Vertex::position) + sizeof(Vertex::color)));
            glEnableVertexAttribArray(ATTRIB_LOC_TEXCOORD);

            glVertexAttribPointer(ATTRIB_LOC_NORMAL, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)(sizeof(Vertex::position) + sizeof(Vertex::color) + sizeof(Vertex::tex_coord)));
            glEnableVertexAttribArray(ATTRIB_LOC_NORMAL);

            elementCount = (GLsizei)elements.size();

            // generating the elements buffer similar to what we did to generate the VBO
            // the only difference is that we bind it to GL_ELEMENT_ARRAY_BUFFER
            glGenBuffers(1, &EBO);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,EBO);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, elementCount * sizeof(unsigned int), elements.data(), GL_STATIC_DRAW);
        }

        // this function should render the mesh
        void draw() 
        {
            //TODO: (Req 1) Write this function
            /**
             * to draw the mesh we need 2 things
             * 1- binding the VAO so that openGl uses the elements & vertex buffers
             * 2- drawing the elements
             */
            glBindVertexArray(VAO);
            glDrawElements(GL_TRIANGLES,elementCount,GL_UNSIGNED_INT,0);
        }

        // this function should delete the vertex & element buffers and the vertex array object
        ~Mesh(){
            //TODO: (Req 1) Write this function
            glDeleteBuffers(1,&VBO);
            glDeleteBuffers(1,&EBO);
        }

        Mesh(Mesh const &) = delete;
        Mesh &operator=(Mesh const &) = delete;
    };

}
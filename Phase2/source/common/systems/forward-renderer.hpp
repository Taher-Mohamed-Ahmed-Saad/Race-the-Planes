#pragma once

#include "../ecs/world.hpp"
#include "../components/camera.hpp"
#include "../components/mesh-renderer.hpp"
#include "../asset-loader.hpp"

#include <glad/gl.h>
#include <vector>
#include <algorithm>

namespace our
{
    
    // The render command stores command that tells the renderer that it should draw
    // the given mesh at the given localToWorld matrix using the given material
    // The renderer will fill this struct using the mesh renderer components
    struct RenderCommand {
        glm::mat4 localToWorld;
        glm::vec3 center;
        Mesh* mesh;
        Material* material;
    };

    // A forward renderer is a renderer that draw the object final color directly to the framebuffer
    // In other words, the fragment shader in the material should output the color that we should see on the screen
    // This is different from more complex renderers that could draw intermediate data to a framebuffer before computing the final color
    // In this project, we only need to implement a forward renderer
    class ForwardRenderer {
        // These window size will be used on multiple occasions (setting the viewport, computing the aspect ratio, etc.)
        glm::ivec2 windowSize;
        // These are two vectors in which we will store the opaque and the transparent commands.
        // We define them here (instead of being local to the "render" function) as an optimization to prevent reallocating them every frame
        std::vector<RenderCommand> opaqueCommands;
        std::vector<RenderCommand> transparentCommands;
        // Objects used for rendering a skybox
        Mesh* skySphere;
        TexturedMaterial* skyMaterial;
        // Objects used for Postprocessing
        GLuint postprocessFrameBuffer, postProcessVertexArray;
        Texture2D *colorTarget, *depthTarget;
        TexturedMaterial* postprocessMaterial;


    public:
        // colors of sky light
        glm::vec3 topLight=glm::vec3(0.0,0.0,0.0);
        glm::vec3 middleLight = glm::vec3(0.0, 0.0, 0.0);
        glm::vec3 bottomLight = glm::vec3(0.0, 0.0, 0.0);
        // Initialize the renderer including the sky and the Postprocessing objects.
        // windowSize is the width & height of the window (in pixels).
        void initialize(glm::ivec2 windowSize, const nlohmann::json& config);
        // Clean up the renderer
        void destroy();
        // This function should be called every frame to draw the given world
        void render(World* world);

        void deserialize(const nlohmann::json &data) 
        {
            if (data.contains("topLight"))
            {
                std::vector<float> v=data["topLight"];
                topLight = glm::vec3(v[0], v[1], v[2]);
                std::cout << "Got Top Light [ " << v[0] << " ," << v[1] << " ," << v[2] << " ]\n\n\n";
            }
            if(data.contains("middleLight"))
            {
                std::vector<float> v=data["middleLight"];
                middleLight = glm::vec3(v[0], v[1], v[2]);
                std::cout << "Got middle Light [ " << v[0] << " ," << v[1] << " ," << v[2] << " ]\n\n\n";
            }
            if(data.contains("bottomLight"))
            {
                std::vector<float> v=data["bottomLight"];
                bottomLight = glm::vec3(v[0], v[1], v[2]);
            }
            
        }
    };

}
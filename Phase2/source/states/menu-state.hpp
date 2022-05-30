#pragma once

#include <application.hpp>

#include <ecs/world.hpp>
#include <systems/forward-renderer.hpp>
#include <systems/free-camera-controller.hpp>
#include <systems/movement.hpp>
#include <systems/plane-controller.hpp>
#include <asset-loader.hpp>

// This state shows how to use the ECS framework and deserialization.
class MenuState : public our::State
{
    our::World world;
    our::ForwardRenderer renderer;
    void onInitialize() override
    {
        // First of all, we get the scene configuration from the app config
        auto &config = getApp()->getConfig()["scene"];
        // If we have assets in the scene config, we deserialize them
        if (config.contains("assets"))
        {
            our::deserializeAllAssets(config["assets"]);
        }
        // If we have a world in the scene config, we use it to populate our world
        if (config.contains("world"))
        {
            world.deserialize(config["world"]);
        }

        // Then we initialize the renderer
        auto size = getApp()->getFrameBufferSize();
        renderer.initialize(size, config["renderer"]);
    }
    void onImmediateGui() override
    {
        ImGui::Begin("Plane Game");
        
        if(ImGui::Button("Start Game")) 
        {
            getApp()->changeState("game");
        }

        if(ImGui::Button("Quit Game")){
            glfwSetWindowShouldClose(getApp()->getWindow(),1);
        }
        ImGui::End();
    }

    void onDraw(double deltaTime) override
    {
        renderer.render(&world);
    }


    void onDestroy() override
    {
        renderer.destroy();
        // and we delete all the loaded assets to free memory on the RAM and the VRAM
        our::clearAllAssets();
    }
};
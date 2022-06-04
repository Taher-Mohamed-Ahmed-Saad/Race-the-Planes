#pragma once

#include <application.hpp>

#include <ecs/world.hpp>
#include <systems/text-renderer.hpp>
#include <systems/forward-renderer.hpp>
#include <systems/free-camera-controller.hpp>
#include <systems/movement.hpp>
#include <systems/plane-controller.hpp>
#include <systems/collision-detector.hpp>
#include <systems/plane-game.hpp>
#include <asset-loader.hpp>
#include "menu-state.hpp"

// This state shows how to use the ECS framework and deserialization.
class GameState: public our::State {

    our::World world;
    our::ForwardRenderer renderer;
    our::TextRenderer textRenderer;
    our::FreeCameraControllerSystem cameraController;
    our::MovementSystem movementSystem;
    our::CollisionDetector collisionDetector;
    our::PlaneControllerSystem planeSystem;
    our::PlaneGame game;
    bool loaded=false;
    bool isMenuRegistered=false;
    void onInitialize() override {
        if(loaded) return;
        // First of all, we get the scene configuration from the app config
        auto& config = getApp()->getConfig()["scene"];
        // If we have assets in the scene config, we deserialize them
        if(config.contains("assets")){
            our::deserializeAllAssets(config["assets"]);
        }
        // If we have a world in the scene config, we use it to populate our world
        if(config.contains("world")){
            world.deserialize(config["world"]);
        }
        planeSystem.enter(getApp());
        cameraController.enter(getApp());

        // Then we initialize the renderer
        auto size = getApp()->getFrameBufferSize();
        renderer.initialize(size, config["renderer"]);
        font = our::AssetLoader<our::Font>::get("default");
        textRenderer.initialize(getApp()->getWindowSize(), our::AssetLoader<our::ShaderProgram>::get(config.value("shader", "text")));
        game.initialize(config["game"]);
        loaded=true;
    }
    our::Font* font;
    void onDraw(double deltaTime) override
    {
        if(!game.gameEnded()){
            // Here, we just run a bunch of systems to control the world logic
            movementSystem.update(&world, (float)deltaTime);
            planeSystem.update(&world, (float)deltaTime);
            
            auto collisions = collisionDetector.update(&world);
            game.update(&world, collisions, deltaTime);
            collisions.clear();
            world.deleteMarkedEntities();
            cameraController.update(&world,(float)deltaTime);
            
        }
        renderer.render(&world);
        // And finally we use the renderer system to draw the scene



        //** Render Text Score and Time
        glm::vec2 scorePosition=  glm::vec2(getApp()->getWindowSize().x*0.9, getApp()->getWindowSize().y*0.9);
        std::vector<our::TextRenderCommand> commands = {
            our::TextRenderCommand(font,
                                   "Score: " + std::to_string(game.getScore()),
                                   0.5f,
                                   scorePosition,
                                   glm::vec3(0.2, 0.3, 0.3)),
            our::TextRenderCommand(font,
                                   "Time: " + std::to_string(game.getRemainingTime()),
                                   0.5f,
                                   scorePosition-glm::vec2(0,50),
                                   glm::vec3(0.2, 0.3, 0.3)),
            our::TextRenderCommand(font,
                                   "Rings: " + std::to_string(game.getRemainingRings()),
                                   0.5f,
                                   scorePosition-glm::vec2(0,100),
                                   glm::vec3(0.2, 0.3, 0.3)),
        };
        glm::vec2 centerPosition = glm::vec2(getApp()->getWindowSize().x * 0.5, getApp()->getWindowSize().y * 0.5);
        if (game.hasWonGame())
        {
            glm::vec2 position = centerPosition - glm::vec2(150, 50);
            our::TextRenderCommand command(font,
                                           "You Won !",
                                           2.5f,
                                           position,
                                           glm::vec3(0.2, 0.9, 0.5));
            commands.push_back(command);
        }

        if(game.hasLostGame()){
            glm::vec2 position = centerPosition- glm::vec2(150, 50);
            our::TextRenderCommand command(font,
                                           "Game Over",
                                           2.5f,
                                           position,
                                           glm::vec3(0.9, 0.2, 0.5)
                                           );
            commands.push_back(command);
        }
        textRenderer.render(commands);
        if(game.gameEnded()&&!isMenuRegistered){
            getApp()->registerState<MenuState>("menu");
            isMenuRegistered = true;
        }

        if (getApp()->getKeyboard().isPressed(GLFW_KEY_ESCAPE))
        {
            getApp()->changeState("menu");
        }
    }

    ~GameState()  {
        // Don't forget to destroy the renderer
        renderer.destroy();
              // On exit, we call exit for the camera controller system to make sure that the mouse is unlocked
        cameraController.exit();
        // and we delete all the loaded assets to free memory on the RAM and the VRAM
        our::clearAllAssets();
        cameraController.exit();
    }
};
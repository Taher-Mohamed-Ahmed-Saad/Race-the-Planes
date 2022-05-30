#pragma once

#include <application.hpp>

#include <ecs/world.hpp>
#include <systems/forward-renderer.hpp>
#include <systems/free-camera-controller.hpp>
#include <systems/movement.hpp>
#include <systems/plane-controller.hpp>
#include <systems/collision-detector.hpp>
#include <systems/plane-game.hpp>
#include <asset-loader.hpp>

// This state shows how to use the ECS framework and deserialization.
class GameState: public our::State {

    our::World world;
    our::ForwardRenderer renderer;
    our::FreeCameraControllerSystem cameraController;
    our::MovementSystem movementSystem;
    our::CollisionDetector collisionDetector;
    our::PlaneControllerSystem planeSystem;
    our::PlaneGame game;

    void onInitialize() override {
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
    }

    void onDraw(double deltaTime) override {
        // Here, we just run a bunch of systems to control the world logic
        movementSystem.update(&world, (float)deltaTime);
        planeSystem.update(&world, (float)deltaTime);
        
        auto collisions = collisionDetector.update(&world);
        game.update(&world,collisions);
        collisions.clear();
        world.deleteMarkedEntities();
        cameraController.update(&world,(float)deltaTime);
        // And finally we use the renderer system to draw the scene
        renderer.render(&world);
    }

    void onDestroy() override {
        // Don't forget to destroy the renderer
        renderer.destroy();
              // On exit, we call exit for the camera controller system to make sure that the mouse is unlocked
        // cameraController.exit();
        // and we delete all the loaded assets to free memory on the RAM and the VRAM
        our::clearAllAssets();
        cameraController.exit();
    }
};
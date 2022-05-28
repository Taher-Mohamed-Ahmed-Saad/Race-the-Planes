#pragma once

#include "../ecs/world.hpp"
#include "../components/plane-component.hpp"
#include "../application.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/trigonometric.hpp>
#include <glm/gtx/fast_trigonometry.hpp>

namespace our
{
    glm::vec2 clampMagnitude(glm::vec2 vector,float magnitude){
        if(glm::length(vector)>magnitude){
            return glm::normalize(vector) * magnitude;
        }
        return vector;
    }

    float lerp(float a,float b,float t){
        return a +(b-a)*t;
    }
    
    class PlaneControllerSystem {
            Application *app;          // The application in which the state runs
        public:
            // glm::vec3 speed = {7.5,5,25.0};
            // float forwardSpeed=25.0,hoverSpeed=10,steerSpeed=15;
            // glm::vec3 acceleration={2,2,2.5};
            // float lookRateSpeed=1;
            // glm::vec2 lookInput, , mouseDistance;
            // float rollInput;
            // float rollSpeed=1.5,rollAcceleration=1;
            glm::vec2 screenCenter ;
             void enter(Application * app)
            {
                this->app = app;
                screenCenter=app->getWindowSize()/2;
                // app->getMouse().lockMouse(app->getWindow());
            }

            void update(World *world, float deltaTime)
            {
                for (auto entity : world->getEntities())
                {   
                    float verticalAxis = 0,horizontalAxis=0,hoverAxis=0,rollAxis=0;
                    if(app->getKeyboard().isPressed(GLFW_KEY_W)) verticalAxis+=1;
                    if(app->getKeyboard().isPressed(GLFW_KEY_S)) verticalAxis-=1;

                    if(app->getKeyboard().isPressed(GLFW_KEY_A)) horizontalAxis+=1;
                    if(app->getKeyboard().isPressed(GLFW_KEY_D)) horizontalAxis -= 1;

                    if(app->getKeyboard().isPressed(GLFW_KEY_SPACE)) hoverAxis+=1;
                    if(app->getKeyboard().isPressed(GLFW_KEY_LEFT_CONTROL)) hoverAxis -= 1;

                    if(app->getKeyboard().isPressed(GLFW_KEY_Q)) rollAxis+=1;
                    if(app->getKeyboard().isPressed(GLFW_KEY_E)) rollAxis -= 1;

                    PlaneComponent *plane = entity->getComponent<PlaneComponent>();
                    if (plane)
                    {
                        Transform* transform = &entity->localTransform;
                        
                        glm::vec2 lookInput = app->getMouse().getMousePosition();
                        glm::vec2 mouseDistance = (lookInput - screenCenter) / screenCenter.y;

                        mouseDistance = clampMagnitude(mouseDistance, 1);
                        plane->rollInput=lerp(plane->rollInput,rollAxis,plane->rollAcceleration*deltaTime);

                        double rotationX = mouseDistance.y * plane->maxLookRateSpeed ;
                        double rotationY = -mouseDistance.x * plane->maxLookRateSpeed ;
                        double rotationZ = plane->rollInput * plane->maxRollSpeed ;
                        
                        transform->rotation = glm::vec3(rotationX, rotationY, rotationZ);

                        auto M=transform->toMat4();
                        glm::vec3 planeForward  = glm::normalize(glm::vec3(M * glm::vec4(0,0,1,0)));
                        glm::vec3 planeUp       = glm::normalize(glm::vec3(M * glm::vec4(0,1,0,0)));
                        glm::vec3 planeSteer    = glm::normalize(glm::vec3(M * glm::vec4(1,0,0,0)));

                        plane->velocity.x = lerp(plane->velocity.x, horizontalAxis  *plane->maxSteerSpeed     ,plane->acceleration.x * deltaTime);
                        plane->velocity.y = lerp(plane->velocity.y, hoverAxis       *plane->maxHoverSpeed     ,plane->acceleration.y * deltaTime);
                        plane->velocity.z = lerp(plane->velocity.z, verticalAxis    *plane->maxForwardSpeed   ,plane->acceleration.z * deltaTime);

                        transform->position+=planeForward   *plane->velocity.z * deltaTime
                                                        +planeUp        *plane->velocity.y * deltaTime
                                                        +planeSteer     *plane->velocity.x * deltaTime;
                    }

                }
            }

            ~PlaneControllerSystem(){
                app->getMouse().unlockMouse(app->getWindow());
            }
    };

}

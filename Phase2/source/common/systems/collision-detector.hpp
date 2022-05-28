#pragma once

#include "../ecs/world.hpp"
#include "../components/collision.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/trigonometric.hpp>
#include <glm/gtx/fast_trigonometry.hpp>

namespace our
{
    class CollisionDetector
    {

        bool areCollidingBoxSphere(BoxCollider *boxCollider, SphereCollider *sphereCollider,glm::mat4 boxMat,glm::vec3 spherePos)
        {   
            // to check if box and sphere are colliding we need to check if any point of the box is inside the sphere.
            // or if the sphere inside the box.
            float w=boxCollider->width, h=boxCollider->height,d=boxCollider->depth;
            glm::vec3 boxPos=glm::vec3(boxMat*glm::vec4(0,0,0,1));

            std::vector<glm::vec3> boxPoints={
                glm::vec3(w/2,h/2,-d/2),
                glm::vec3(w/2,h/2,d/2),
                glm::vec3(w/2,-h/2,d/2),
                glm::vec3(w/2,-h/2,-d/2),
                glm::vec3(-w/2,h/2,d/2),
                glm::vec3(-w/2,h/2,-d/2),
                glm::vec3(-w/2,-h/2,d/2),
                glm::vec3(-w/2,-h/2,-d/2)
            };
            
            for(int i=0; i<boxPoints.size();i++){
                boxPoints[i] = boxMat *glm::vec4(boxPoints[i],1);
                if (glm::distance(boxPoints[i], spherePos) < sphereCollider->radius) return true;
            }
            
            return false;
        }

        bool areCollidingSpheres(SphereCollider *collider1, SphereCollider *collider2,glm::vec3 pos1,glm::vec3 pos2)
        {   
            return (glm::distance(pos1,pos2) < collider1->radius+collider2->radius);
        }

        bool areColliding(Entity *entity1, Entity *entity2)
        {
            Collider* collider1=entity1->getComponent<CollisionComponent>()->collider;
            Collider* collider2=entity2->getComponent<CollisionComponent>()->collider;
            if (!collider1 || !collider2 || collider2 == collider1)
                return false;

            glm::vec3 position1 = entity1->localTransform.position;
            glm::vec3 position2 = entity2->localTransform.position;   

            SphereCollider *sphere1 = dynamic_cast<SphereCollider *>(collider1);
            SphereCollider *sphere2 = dynamic_cast<SphereCollider *>(collider2);
            
            if(sphere1&&sphere2)
                return areCollidingSpheres(sphere1, sphere2, position1, position2);

            BoxCollider *box1 = dynamic_cast<BoxCollider *>(collider1);
            BoxCollider *box2 = dynamic_cast<BoxCollider *>(collider2);
            
            if(sphere1&&box2)
                return areCollidingBoxSphere(box2, sphere1, entity2->localTransform.toMat4(), position1);
    
            if(sphere2&&box1)
                return areCollidingBoxSphere(box1, sphere2, entity1->localTransform.toMat4(), position2);

            return false;
        }

    public:
        void update(World *world)
        {
            std::vector<Entity*> staticEntities;
            std::vector<Entity*> animatedEntities;
            // For each entity in the world
            for (auto entity : world->getEntities())
            {
                // Get the movement component if it exists
                CollisionComponent *collision = entity->getComponent<CollisionComponent>();
                // If the movement component exists
                if (collision)
                {
                    if(collision->type==CollisionType::STATIC){
                        staticEntities.push_back(entity);
                    }
                    if(collision->type==CollisionType::DYNAMIC){
                        //because dynamic objects can collide with each other we will add 
                        // the collider to both lists.
                        staticEntities.push_back(entity);
                        animatedEntities.push_back(entity);
                    }
                }
            }


            for(auto staticEntity :staticEntities){
                for (auto animatedEntity : animatedEntities)
                {
                    if(areColliding(staticEntity,animatedEntity)){
                        std::cout<<"Coooooooooolision\n\n\n";
                }
            }
        }
    };

};
}

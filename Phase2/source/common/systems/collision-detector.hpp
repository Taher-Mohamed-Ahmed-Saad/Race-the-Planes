#pragma once
#include<cmath>
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
        
        //return box points in world space
        std::vector<glm::vec3> getBoxPoints(BoxCollider *boxCollider, glm::mat4 boxMat){
            float w = boxCollider->width, h = boxCollider->height,d=boxCollider->depth;

            std::vector<glm::vec3> points={
                {w/2,h/2,d/2},
                {w/2,h/2,-d/2},
                {w/2,-h/2,d/2},
                {w/2,-h/2,-d/2},
                {-w/2,h/2,d/2},
                {-w/2,h/2,-d/2},
                {-w/2,-h/2,d/2},
                {-w/2,-h/2,-d/2},
            };

            for(int i=0; i<points.size();i++){
                points[i]= glm::vec3(boxMat*glm::vec4(points[i],1));
            }
            return points;
        }
        // return true if point is inside this box
        // point is in world space
        bool isInside(BoxCollider *boxCollider ,glm::mat4 boxMat, glm::vec3 point ,float offset){
            // local axis of the box we will use them to get the distance to the center of the point in those directions
            glm::vec3 localBoxX = boxMat * glm::vec4(1, 0, 0, 0);
            glm::vec3 localBoxY = boxMat * glm::vec4(0, 1, 0, 0);
            glm::vec3 localBoxZ = boxMat * glm::vec4(0, 0, 1, 0);
            glm::vec3 origin = boxMat * glm::vec4(0, 0, 0, 1);

            glm::vec3 pointPosRelativeToBox = boxMat * glm::vec4(point - origin, 0);
            // distance between the box center and the point center in the direction of localX
            float localDistanceX = glm::dot(localBoxX, pointPosRelativeToBox);
            // distance between the box center and the point center in the direction of localY
            float localDistanceY = glm::dot(localBoxY, pointPosRelativeToBox);
            // distance between the box center and the point center in the direction of localY
            float localDistanceZ = glm::dot(localBoxZ, pointPosRelativeToBox);

            // collision will happen if the distance between the point center and the box center
            // is less than r + w/2 in local x direction and
            // is less than r + h/2 in local y direction and
            // is less than r + d/2 in local z direction

            return (
                std::abs(localDistanceX) < offset + boxCollider->width / 2 &&
                std::abs(localDistanceY) < offset + boxCollider->height / 2 &&
                std::abs(localDistanceZ) < offset + boxCollider->depth / 2);
        }
        bool areCollidingBoxes(BoxCollider *boxCollider1, glm::mat4 boxMat1, BoxCollider *boxCollider2, glm::mat4 boxMat2)
        {
            // boxes are colliding if a point of one of them is inside the other one

            auto boxPoints1=getBoxPoints(boxCollider1,boxMat1);
            for(auto point:boxPoints1){
                if(isInside(boxCollider2,boxMat2,point,0))
                    return true;
            }

            auto boxPoints2=getBoxPoints(boxCollider2,boxMat2);
            for(auto point:boxPoints2){
                if(isInside(boxCollider1,boxMat1,point,0))
                    return true;
            }
            return false;
        }

        bool areCollidingBoxSphere(BoxCollider *boxCollider, SphereCollider *sphereCollider,glm::mat4 boxMat,glm::vec3 spherePos)
        {   
           return isInside(boxCollider,boxMat,spherePos,sphereCollider->radius);
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

            if(box1&&box2)
                return areCollidingBoxes(box1, entity1->localTransform.toMat4(), box2, entity2->localTransform.toMat4());

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

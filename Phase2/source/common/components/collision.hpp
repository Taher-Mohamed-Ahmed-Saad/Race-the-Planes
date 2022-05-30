#pragma once

#include "../ecs/component.hpp"
#include "../colliders/collider.hpp"

namespace our
{
    enum CollisionType {
        STATIC,DYNAMIC
    };
    class Collision
    {
    public:
        Entity *entity1 = nullptr;
        Entity *entity2 = nullptr;
        public:
            Collision(Entity *entity1, Entity *entity2)
            {
                this->entity1 = entity1;
                this->entity2 = entity2;
            }
            template <typename T>
            T *getComponent()
            {
                T *comp = entity1->getComponent<T>();
                if (comp)
                    return comp;

                comp = entity2->getComponent<T>();
                return comp;
            }
    };

    class CollisionComponent : public Component
    {   
        public:
            Collider* collider;
            CollisionType type;
            static std::string getID() { return "Collision"; }

            // Reads Collision parameters from the given json object
            void deserialize(const nlohmann::json &data) override;

    };

}
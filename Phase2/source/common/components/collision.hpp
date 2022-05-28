#pragma once

#include "../ecs/component.hpp"
#include "../colliders/collider.hpp"

namespace our
{
    enum CollisionType {
        STATIC,DYNAMIC
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
#include "collision.hpp"
#include "../colliders/collider-deserializer.cpp"

namespace our{
    void CollisionComponent::deserialize(const nlohmann::json &data){
        std::string typeStr=data.value("collisionType","static");
        if(typeStr=="static")
            type = CollisionType::STATIC;
        if(typeStr=="dynamic")
            type = CollisionType::DYNAMIC;
        
        std::string colliderType=data.value("collisionType","static");
        collider= deserializeCollider(data["collider"]);
    }
}
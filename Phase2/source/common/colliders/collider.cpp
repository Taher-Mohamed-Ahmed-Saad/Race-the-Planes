#include "collider.hpp"

namespace our{

    void SphereCollider::deserialize(const nlohmann::json &data){
        radius = data.value("radius", radius);
    }
    
    void BoxCollider::deserialize(const nlohmann::json &data){
        width = data.value("width", width);
        height = data.value("height", height);
        depth = data.value("depth", depth);
    }
    
}
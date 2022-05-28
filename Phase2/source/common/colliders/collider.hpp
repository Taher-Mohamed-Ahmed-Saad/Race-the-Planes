#pragma once

#include <json/json.hpp>
#include <string>

namespace our{

    class SphereCollider;

    class Collider{
        public:
            virtual void deserialize(const nlohmann::json &data)=0;
    };

    class SphereCollider : public Collider
    {
        public:
            float radius=1.0f;
            static std :: string getID(){return "Sphere";}
            void deserialize(const nlohmann::json &data) override;
    };
    
    class BoxCollider : public Collider
    {
        public:
            float width=1.0f;
            float height=1.0f;
            float depth=1.0f;
            
            static std :: string getID(){return "Box";}
            void deserialize(const nlohmann::json &data) override;
    };
    

}
#pragma once
#include "../ecs/component.hpp"
namespace our{
    class PlaneComponent :public Component{
        public:
            glm::vec3 acceleration={2,2,2.5};
            glm::vec3 velocity = {0, 0, 0};

            float maxForwardSpeed=25.0,maxHoverSpeed=10,maxSteerSpeed=15;
            float maxLookRateSpeed=1;
            
            float maxRollSpeed=1.5,rollAcceleration=1.5;
            float rollInput=0;
            static std::string getID() { return "Plane"; }
            void deserialize(const nlohmann::json &data) override;
    };
} 
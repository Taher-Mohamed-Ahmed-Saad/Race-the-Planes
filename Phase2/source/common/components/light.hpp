#pragma once

#include "../ecs/component.hpp"

namespace our{
    enum class LightType{
        DIRECTIONAL = 0,POINT =1, SPOT = 2
    };
    

    class Light : public Component{
        public:
            LightType lightType;
            glm::vec3 diffuse=glm::vec3(0.0,0.0,0.0);
            glm::vec3 specular = glm::vec3(0.0, 0.0, 0.0);
            glm::vec3 attenuation = glm::vec3(1.0,0.0,0.0);;
            glm::vec2 cone_angles = glm::vec2(45, 90);

            static std::string getID() { return "Light"; }
            // Reads light parameters from the given json object
            void deserialize(const nlohmann::json &data) override;
    };
}
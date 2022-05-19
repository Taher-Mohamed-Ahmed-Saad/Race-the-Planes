#include "light.hpp"

namespace our{

    void Light::deserialize(const nlohmann::json &data)
    {
        if(!data.is_object()) return ;
        std::string lightTypeStr = data.value("lightType", "directional");
        
        if(lightTypeStr=="point"){
            lightType=LightType::POINT;
            attenuation = data.value("attenuation", attenuation);
        }else if (lightTypeStr=="spot"){
            lightType=LightType::SPOT;
            cone_angles = data.value("cone_angles", cone_angles);
        }else if (lightTypeStr=="directional"){
            lightType=LightType::DIRECTIONAL;
        }

        diffuse = data.value("diffuse", diffuse);
        specular = data.value("specular", specular);
    }
}
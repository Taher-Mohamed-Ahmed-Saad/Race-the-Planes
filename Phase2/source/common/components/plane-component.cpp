#include "plane-component.hpp" 

namespace our
{

    void PlaneComponent::deserialize(const nlohmann::json &data)
    {
        if (!data.is_object())
            return;
        acceleration = data.value("acceleration", acceleration);
        maxForwardSpeed = data.value("maxForwardSpeed", maxForwardSpeed);
        maxHoverSpeed = data.value("maxHoverSpeed", maxHoverSpeed);
        maxSteerSpeed = data.value("maxSteerSpeed", maxSteerSpeed);
        maxLookRateSpeed = data.value("maxLookRateSpeed", maxLookRateSpeed);
        maxRollSpeed = data.value("maxRollSpeed", maxRollSpeed);
        rollAcceleration = data.value("rollAcceleration", rollAcceleration);
    }
}
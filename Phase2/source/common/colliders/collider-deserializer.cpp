#include"collider.cpp"

namespace our{
    Collider *deserializeCollider(const nlohmann::json &data)
    {
        std::string type = data.value("type", "");
        Collider *collider = nullptr;
        if (type == SphereCollider::getID())
            collider = new SphereCollider();
        if (type == BoxCollider::getID())
            collider = new BoxCollider();

        collider->deserialize(data);
        return collider;
    }
}
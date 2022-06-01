#pragma once

#include "../ecs/component.hpp"

namespace our{
     class Player : public Component{
        public:
            static std::string getID() { return "Player"; }

            // Reads Player parameters from the given json object
            void deserialize(const nlohmann::json &data){};
     };
     
     class Obstacle : public Component{
        public:
         static std::string getID() { return "Obstacle"; }

         // Reads Player parameters from the given json object
         void deserialize(const nlohmann::json &data){};
     };
     class Coin : public Component{
        public:
            int value=1;
            static std::string getID() { return "Coin"; }

            // Reads Player parameters from the given json object
            void deserialize(const nlohmann::json &data){
                value=data.value("value",value);
            };
     };
}
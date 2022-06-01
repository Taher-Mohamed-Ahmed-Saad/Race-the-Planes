#pragma once
#include <cmath>
#include "../ecs/world.hpp"
#include "../components/collision.hpp"
#include "../components/game-components.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/trigonometric.hpp>
#include <glm/gtx/fast_trigonometry.hpp>

namespace our
{

    class PlaneGame
    {
        int score=0;
        bool lost=false;
        double seconds=30;
        int left=50;
        bool hasLost=false;
        bool hasWon=false;
        public:
            int getScore(){
                return score;
            }
            int getRemainingRings(){
                return left;
            }
            int getRemainingTime(){
                return (int)seconds;
            }
            bool hasWonGame(){
                return hasWon;
            }
            bool hasLostGame(){
                return hasLost;
            }
            bool gameEnded(){
                return hasLost||hasWon;
            }
            void initialize( const nlohmann::json &config){
                left=config.value("collectToWin",left);
                seconds = config.value("time", seconds);
            }
            void update(World *world,std::vector<Collision*> collisions,double deltaTime)
            {
                seconds-=deltaTime;
                if(seconds<=0) hasLost=true;
                for (auto collision : collisions){
                    Player* player=collision->getComponent<Player>();
                    if(!player) continue;

                    Obstacle *obstacle = collision->getComponent<Obstacle>();
                    if (obstacle)
                    {
                        lost=true;
                        hasLost=true;
                    }
                    
                    Coin *coin = collision->getComponent<Coin>();
                    if (coin)
                    {
                        score+=coin->value;
                        Coin * coin = collision->entity1->getComponent<Coin>();
                        if (coin)
                            world->markForRemoval(collision->entity1);

                        coin = collision->entity2->getComponent<Coin>();
                        if (coin)
                            world->markForRemoval(collision->entity2);
                        left--;

                        if(!left) 
                            hasWon=true;
                    }
                }
            }
    };
}

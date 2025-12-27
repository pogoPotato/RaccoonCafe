#pragma once
#include <SDL3/SDL.h>

enum class CustomerState
{
    ENTERING,      //walkinng to cafe
    WAITING,       // waiting to give order .. me click he give
    ORDER_TAKEN,   //after me click order taken
    WAITING_FOOD,  // me cooking
    ORDER_READY,   // waiting to eat
    PAYING,        // paisa gandai xa malai dina
    LEAVING,       // gayooo 
    INACTIVE       // empty shit no order
};

struct Customer {
    SDL_FRect rect;
    CustomerState state;
    float speed;
    float targetX;
    int direction;   // -1 = left .. 1 = right
    bool active;

    float waitTimer;

    // PATIENCE SYSTEM
    float patience;     // on going patience 
    float maxPatience;  //total patience

    // QUEUE SYSTEM
    int queueIndex;     // 0 = at counter .. 1+ = waiting in line
};

// yelpppp
bool isCustomerClicked(const Customer& c, float mx, float my);
void drawCustomer(SDL_Renderer* renderer, const Customer& c);
void updateCustomer(Customer& c, float dt);
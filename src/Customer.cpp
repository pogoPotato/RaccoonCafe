#include "Customer.h"

bool isCustomerClicked(const Customer& c, float mx, float my)
{
    //if active click else fuck offf..
    return (c.active && c.state != CustomerState::INACTIVE) &&
        mx >= c.rect.x && mx <= c.rect.x + c.rect.w &&
        my >= c.rect.y && my <= c.rect.y + c.rect.h;
}

void updateCustomer(Customer& c, float dt)
{
    if (!c.active || c.state == CustomerState::INACTIVE) return;

    //move and shit
    if (c.state == CustomerState::ENTERING)
    {
        c.rect.x += c.speed * c.direction * dt;

        //chceak is arrived, target and shit like whre to go..
        if ((c.direction == 1 && c.rect.x >= c.targetX) ||
            (c.direction == -1 && c.rect.x <= c.targetX))
        {
            c.rect.x = c.targetX;
            c.state = CustomerState::WAITING;
        }
    }
    else if (c.state == CustomerState::LEAVING)
    {
        c.rect.x += c.speed * c.direction * dt;

        // cleanup yeha they make a mess 
        if (c.rect.x < -200 || c.rect.x > 1400)
        {
            c.state = CustomerState::INACTIVE;
            c.active = false;
        }
    }

    // Note: waiting timer food and otehr shit are in game.cpp dude 
    // because these shit rely on the global game state/menu.
}

void drawCustomer(SDL_Renderer* renderer, const Customer& c)
{
    if (!c.active || c.state == CustomerState::INACTIVE) return;

    // assigning colours for liek debug and shit ? waiting comming received leaving typoe shit
    switch (c.state)
    {
    case CustomerState::ENTERING:
        SDL_SetRenderDrawColor(renderer, 150, 50, 250, 255); // Purple (Walking in)
        break;
    case CustomerState::WAITING:
        SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);  // Yellow (Needs you to click)
        break;
    case CustomerState::ORDER_TAKEN:
    case CustomerState::WAITING_FOOD:
        SDL_SetRenderDrawColor(renderer, 100, 100, 100, 255); // Gray (Waiting for food)
        break;
    case CustomerState::ORDER_READY:
        SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);    // Green (Food is ready!)
        break;
    case CustomerState::PAYING:
        SDL_SetRenderDrawColor(renderer, 255, 215, 0, 255);  // Gold (Paying money)
        break;
    case CustomerState::LEAVING:
        SDL_SetRenderDrawColor(renderer, 200, 100, 100, 255); // Reddish (Walking out)
        break;
    default:
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        break;
    }

    SDL_RenderFillRect(renderer, &c.rect);
}
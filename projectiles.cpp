#include "projectiles.h"
#include <math.h>


Projectile::Projectile(const Projectile& other)
{
    x = other.x;
    y = other.y;
    target_x = other.target_x;
    target_y = other.target_y;
    speed_x = other.speed_x;
    speed_y = other.speed_y;
    cooldown = other.cooldown;
    type = other.type;
    erase = other.erase;
    pop = other.pop;
}


Projectile::Projectile(float x0, float y0, float x1, float y1, int atype)
{
    x = x0;
    y = y0;
    target_x = x1;
    target_y = y1;
    type = atype;
    cooldown = 0.75f;
    erase = false;
    pop = false;

    if (type == 2)
    {
        speed_x = -96.f * sinf(atan2(x - target_x, y - target_y));
        speed_y = -96.f * cosf(atan2(x - target_x, y - target_y));
    }

    if (type == 3)
    {
        speed_x = -196.f * sinf(atan2(x - target_x, y - target_y));
        speed_y = -196.f * cosf(atan2(x - target_x, y - target_y));
    }    
}


void Projectile::Update(float dt)
{
    if (type == 1 || type == 4 || type == 5)
    {
        cooldown -= dt;
        if (cooldown < 0)
        {
            erase = true;
        }
    }

    if (type == 2 || type == 3)
    {
        if (fabs(x - target_x) < fabs(speed_x * dt) && fabs(y - target_y) < fabs(speed_y * dt))
        {
            type = type + 2;
            cooldown = 0.75f;
            pop = true;
        }
        else 
        {
            x += speed_x * dt;
            y += speed_y * dt;
        }
    }
}

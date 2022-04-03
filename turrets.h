#pragma once


#include "creeps.h"
#include "projectiles.h"


class Turret
{
public:

    Turret(int i, int j, int Type);

    float x;
    float y;

    int type;

    float fire_cooldown;

    bool CanFire(Creep& creep);
    Projectile Fire(Creep& creep);
};
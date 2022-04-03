#include "turrets.h"


Turret::Turret(int i, int j, int Type)
{
    x = i * 42;
    y = j * 42;
    type = Type;
    fire_cooldown = 0.100;
}


bool Turret::CanFire(Creep& creep)
{
    return true;
}


Projectile Turret::Fire(Creep& creep)
{
    //  Laser
    if (type == 1)
    {
        creep.Damage(1);
    }

    fire_cooldown = 2.f;
    return Projectile(x+21, y+21, creep.x, creep.y, type);
}
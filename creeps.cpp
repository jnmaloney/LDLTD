#include "creeps.h"
#include <random>
#include <functional>
std::default_random_engine generator;
std::uniform_int_distribution<int> distribution(0, 33);
auto dice = std::bind ( distribution, generator );


Creep::Creep(int Level)
{
    Type = Level;

    x = 0;
    y = 7 * 42 + 4 + dice();
    speed = 42.f;

    HP = 5;
    MaxHP = 5;

    Dead = false;
    HasHitBase = false;

    if (Level == 2)
    {
        HP = 15;
        MaxHP = 15;
    }
}


void Creep::Damage(int D)
{
    if (Shield > 0)
    {
        Shield -= D;
        if (Shield > 0)
          return;

        D = -Shield;
    }

    HP -= D;

    if (HP <= 0)
    {
        HP = 0;
        Dead = true;
    }
}
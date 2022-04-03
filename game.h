#pragma once

#include "creeps.h"
#include "turrets.h"
#include "projectiles.h"
#include <vector>
#include <map>


class Game
{
public:

    void Reset();

    void Update(float dt);

    void Draw();

    void NextWave();

    std::vector<Creep> Creeps;
    std::vector<Turret> Turrets;
    std::vector<Projectile> Projectiles;

    int Grid[30][15];

    int WaveNumber;
    int WaveTotal;

    int PlayerCash;

    float PlayerShieldHealth;
    float PlayerBaseHealth;

    float PlayerShieldHealthRegen;
    float PlayerBaseHealthRegen;

    int BuildingMode;
    int ToSpend;
};
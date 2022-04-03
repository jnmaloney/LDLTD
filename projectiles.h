#pragma once

class Projectile
{
public:

    Projectile(float x0, float y0, float x1, float y1, int type);
    Projectile(const Projectile& other);

    void Update(float dt);

    int type;
    float x;
    float y;
    float target_x;
    float target_y;
    float speed_x;
    float speed_y;
    int active;
    float cooldown;
    int damage;
    bool erase;
    bool pop;
};
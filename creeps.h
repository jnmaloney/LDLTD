#pragma once

class Creep
{
public:
    Creep(int Level);
    ~Creep() {}

    float x;
    float y;

    float speed;

    int MaxHP;
    int HP;
    int Size;
    int MaxShield;
    int Shield;
    int Value;

    float R, G, B;

    bool Dead;
    bool HasHitBase;
    
    int Type;

    void Damage(int D);
};


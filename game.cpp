#include "game.h"
#include "imgui.h"
#include <algorithm>    // std::remove_if
#include <string>

struct Wave
{
    int MiniCreeps;
    int MidCreeps;
    int MaxCreeps;
    int ShieldRate;
    int CreepTicker;
    int MiniCreepsRound;
    int MidCreepsRound;
    int MaxCreepsRound;
    int Rounds;
    float NextCreep;
    int N;
};

// struct WaveB
// {
//     WaveB() {}
//     WaveB(const WaveB& other) 
//     {
//         MiniCreeps = other.MiniCreeps;
//         MidCreeps = other.MidCreeps;
//         MaxCreeps = other.MaxCreeps;
//         L = other.L;
//         ShieldRate = other.ShieldRate; 
//         Rounds = other.Rounds;
//     }
//     int MiniCreeps;
//     int MidCreeps;
//     int MaxCreeps;
//     int L;
//     int ShieldRate;    
//     int Rounds;
// };

static int Waves[][6] = {
    { 1, 0, 0, 0, 0, 10 },
    { 1, 0, 0, 0, 0, 20 },
    { 1, 0, 0, 0, 0, 30 },
    { 1, 0, 0, 0, 0, 40 },
    { 1, 0, 0, 0, 0, 50 },
    { 1, 0, 0, 0, 0, 60 },
    { 1, 0, 0, 0, 0, 70 },
    { 1, 0, 0, 0, 0, 80 },
    { 1, 0, 0, 0, 0, 90 },
    { 1, 0, 0, 0, 0, 100 },
    { 10, 1, 0, 0, 0, 10 },
    { 6, 1, 0, 0, 0, 20 },
    { 4, 1, 0, 0, 0, 30 },
    { 3, 1, 0, 0, 100, 40 },
    { 2, 1, 0, 0, 100, 50 },
    { 2, 1, 0, 0, 90, 60 },
    { 2, 1, 0, 0, 80, 70 },
    { 2, 1, 0, 0, 70, 80 },
    { 2, 1, 0, 0, 60, 90 },
    { 2, 1, 0, 0, 50, 100 },
    { 20, 2, 1, 0, 40, 10 },
    { 15, 3, 1, 0, 30, 20 },
    { 20, 3, 1, 0, 20, 30 },
    { 10, 4, 1, 0, 10, 40 },
    { 15, 4, 1, 0, 1, 50 },
    { 20, 4, 1, 0, 1, 60 },
    { 20, 5, 1, 0, 1, 70 },
    { 20, 6, 1, 0, 1, 80 },
    { 20, 7, 1, 0, 1, 90 },
    { 20, 8, 1, 0, 1, 100 },
};


void Game::Reset()
{
    Creeps.clear();
    Projectiles.clear();
    Turrets.clear();
    PlayerBaseHealth = 100;
    PlayerShieldHealth = 100;
    PlayerBaseHealthRegen = 0;
    PlayerShieldHealthRegen = 0;
    PlayerCash = 200;
    WaveNumber = 0;
    WaveTotal = 0;
    for (int i = 0; i < 30; i++)
    {
        for (int j = 0; j < 15; j++)
        {
            if (j == 7)
            {
                Grid[i][j] = -1;
            }
            else 
            {
                Grid[i][j] = 0;
            }
        }
    }

    BuildingMode = 0;
}


bool dist(Projectile& i, Creep& j, float D)
{
    float D2 = (i.x - j.x) * (i.x - j.x) + (i.y - j.y) * (i.y - j.y);
    return D2 < D * D;
}
bool dist(Turret& i, Creep& j, float D)
{
    if (j.Dead) return false;
    float D2 = (i.x + 21 - j.x) * (i.x + 21 - j.x) + (i.y + 21 - j.y) * (i.y + 21 - j.y);
    return D2 < D * D;
}

    static Wave s_wave;
void Game::NextWave()
{
    //WaveB& B = Waves[2];//[WaveNumber - 1];
    int* B = Waves[WaveNumber - 1];

    s_wave.CreepTicker = 0;
    s_wave.MiniCreepsRound = B[0];
    s_wave.MidCreepsRound = B[1];
    s_wave.MaxCreepsRound = B[2];
    s_wave.ShieldRate = B[4];
    s_wave.Rounds = B[5];
    s_wave.MiniCreeps = 0;
    s_wave.MidCreeps = 0;
    s_wave.MaxCreeps = 0;
    s_wave.NextCreep = 2.f;
    s_wave.N = (s_wave.MiniCreepsRound + s_wave.MidCreepsRound + s_wave.MaxCreepsRound) * s_wave.Rounds;
}


void Game::Update(float dt)
{
    //
    // Waves
    //
    if (WaveNumber == 0)
    {
        WaveNumber = 1;
        NextWave();
    }

    if (s_wave.CreepTicker >= s_wave.N)
    {
        WaveNumber += 1;
        NextWave();
    }

    s_wave.NextCreep -= dt;
    if (s_wave.NextCreep <= 0.f)
    {
        s_wave.NextCreep += (100.f / (float)s_wave.N);

        if (s_wave.MiniCreeps < s_wave.MiniCreepsRound)
        {
            s_wave.MiniCreeps++;
            Creeps.push_back(
                Creep(1)
            );
        }
        else 
        {
            Creeps.push_back(
                Creep(2)
            );
        }
        if (s_wave.CreepTicker % (s_wave.N / s_wave.Rounds) == 0)
        {
            s_wave.MiniCreeps = 0;
        }
        
        s_wave.CreepTicker += 1;
        if (s_wave.CreepTicker >= s_wave.N)
        {
            // WantsNextWave
            WaveNumber += 1;
            NextWave();
        }
    }

    //
    // Move projectiles
    //
    for (auto& i : Projectiles)
    {
        i.Update(dt);
        if (i.pop)
        {
            i.pop = false;
            for (auto& j : Creeps)
            {
                if (i.type == 2)
                    if (dist(i, j, 42.f))
                    {
                        j.Damage(1);
                    }
                if (i.type == 3)
                    if (dist(i, j, 24.f))
                    {
                        j.Damage(1);
                    }                    
            }
        }
    }

    //
    // Turret Aim and Fire
    //
    for (auto& i : Turrets)
    {
        if (i.fire_cooldown < dt)
        {
            // Fire..?            
            for (auto& j : Creeps)
            {
                if (dist(i, j, 420.f))
                    if (i.CanFire(j))
                    {
                        Projectiles.push_back(i.Fire(j));
                        break;
                    }
            }
        }
        else 
        {
            i.fire_cooldown -= dt;
        }
    }

   
    //
    // Spawn Creep Wave
    //
    // if (Creeps.size() == 0)
    // Creeps.push_back(
    //     Creep()
    // );

    //
    // Move Creeps
    // 
    for (auto& i : Creeps)
    {
        if (i.HasHitBase) continue;
        if (i.Dead) PlayerCash += 5;

        i.x += dt * i.speed;

        if (i.x > 30 * 42)
        {
            i.HasHitBase = true;
            PlayerBaseHealth -= 1;
        }
    }

    //
    // Remove Creeps
    //
    Creeps.erase(
        std::remove_if(Creeps.begin(), Creeps.end(),
            [](const Creep & o) { return o.HasHitBase; }),
        Creeps.end());
    Creeps.erase(
        std::remove_if(Creeps.begin(), Creeps.end(),
            [](const Creep & o) { return o.Dead; }),
        Creeps.end());

    //
    // Remove projectiles
    //
    Projectiles.erase(
        std::remove_if(Projectiles.begin(), Projectiles.end(),
            [](const Projectile & o) { return o.erase; }),
        Projectiles.end());

}

void Game::Draw()
{
    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowSize(ImVec2(1280, 720));
    ImGui::Begin(
        "title", 
        nullptr, 
        ImGuiWindowFlags_NoDecoration |
        ImGuiWindowFlags_NoBackground |
        ImGuiWindowFlags_NoBringToFrontOnFocus);
    
    //
    // Check Game Over
    // 
    if (PlayerBaseHealth <= 0)
    {       

        ImGui::SetCursorPos(ImVec2(240, 240));
        ImGui::Text("Game over after %i waves", (int)WaveNumber);

        ImGui::SetCursorPos(ImVec2(240, 320));
        if (ImGui::Button("Reset##1", ImVec2(746, 64)))
        {
            Reset();
        }

        ImGui::End();
        return;
    }

    //
    // Draw Menu
    // 
    if (BuildingMode == 0)
    {
        ImGui::SetCursorPos(ImVec2(12, 12));
        if (ImGui::Button("Laser Turret\n$100##1", ImVec2(246, 64)))
        {
            if (PlayerCash >= 100)
            {
                ToSpend = 100;
                BuildingMode = 1;
            }
        }

        ImGui::SetCursorPos(ImVec2(270, 12));
        if (ImGui::Button("Plasma Turret\n$400##1", ImVec2(246, 64)))
        {
            if (PlayerCash >= 400)
            {
                ToSpend = 400;
                BuildingMode = 2;
            }
        }

        ImGui::SetCursorPos(ImVec2(528, 12));
        if (ImGui::Button("Ion Turret\n$500##2", ImVec2(246, 64)))
        {
            if (PlayerCash >= 500)
            {
                ToSpend = 500;
                BuildingMode = 3;
            }           
        }
    }
    else 
    {
        ImGui::SetCursorPos(ImVec2(12, 12));
        if (ImGui::Button("Cancel##1", ImVec2(746, 64)))
        {
            BuildingMode = 0;
        }
    }

    ImGui::SetCursorPos(ImVec2(840, 24));
    ImGui::Text("Wave %i", (int)WaveNumber);

    ImGui::SetCursorPos(ImVec2(840, 38));
    ImGui::Text("$%i", (int)PlayerCash);

    ImGui::SetCursorPos(ImVec2(840, 52));
    ImGui::Text("Base Health: %i%%", (int)PlayerBaseHealth);

    //
    // Draw Scene
    //
    ImDrawList* draw_list = ImGui::GetWindowDrawList();
    for (int i = 0; i < 30; i++)
    {
        for (int j = 0; j < 15; j++)
        {
            ImVec2 P0(1 + i * 42, 90 + j * 42);
            ImVec2 P1(43 + i * 42, 132 + j * 42);

            // Draw terrain tile
            if (Grid[i][j] >= 0)
            {
                draw_list->AddRectFilled(P0, P1, IM_COL32(134, 183, 70, 255));
            }
            else 
            {
                draw_list->AddRectFilled(P0, P1, IM_COL32(155, 123, 76, 255));
            }
            //draw_list->AddRect(P0, P1, IM_COL32(255, 255, 155, 255));
        }
    }

    //
    // Draw Turrets
    //
    for (auto& i : Turrets)
    {
        ImVec2 P0(21 + i.x, 110 + i.y);
        draw_list->AddCircleFilled(P0, 18.0, IM_COL32(115, 115, 115, 255));
    }

    // 
    // Draw Creeps
    //
    for (auto& i : Creeps)
    {
        ImVec2 P0(1 + i.x, 90 + i.y);
        if (i.Type == 1)
        {
            float C = 0.75f + 0.25f * ((float)i.HP / (float)i.MaxHP);
            draw_list->AddCircleFilled(P0, 5.0, IM_COL32(222*C, 229*C, 115*C, 255));
        }
        else 
        {
            float C = 0.75f + 0.25f * ((float)i.HP / (float)i.MaxHP);
            draw_list->AddCircleFilled(P0, 6.0, IM_COL32(223*C, 184*C, 193*C, 255));
        }
    }

    //
    // Draw Projectiles
    //
    for (auto& i : Projectiles)
    {
        if (i.type == 1)
        {
            ImVec2 P0(1 + i.x, 90 + i.y);
            ImVec2 P1(1 + i.target_x, 90 + i.target_y);
            
            int C = 225 * (i.cooldown/0.75) * (i.cooldown/0.75) * (i.cooldown/0.75) * (i.cooldown/0.75);
            draw_list->AddLine(P0, P1, IM_COL32(229, 229, 229, C), 2.f); 
        }

        if (i.type == 2)
        {
            ImVec2 P0(i.x - 4, 90 + i.y - 4);
            ImVec2 P1(i.x + 4, 90 + i.y + 4);
            
            draw_list->AddRectFilled(P0, P1, IM_COL32(229, 229, 229, 255));
        }

        if (i.type == 3)
        {
            ImVec2 P0(i.x - 3, 90 + i.y - 3);
            ImVec2 P1(i.x + 3, 90 + i.y + 3);
            
            draw_list->AddRectFilled(P0, P1, IM_COL32(209, 209, 255, 255));
        }  

        if (i.type == 4)
        {
            ImVec2 P0(1 + i.x, 90 + i.y);
            int C = 125 * (i.cooldown/0.75) * (i.cooldown/0.75) * (i.cooldown/0.75) * (i.cooldown/0.75);
            draw_list->AddCircleFilled(P0, 42.0, IM_COL32(229, 229, 229, C));
        }        

        if (i.type == 5)
        {
            ImVec2 P0(1 + i.x, 90 + i.y);
            int C = 125 * (i.cooldown/0.75) * (i.cooldown/0.75) * (i.cooldown/0.75) * (i.cooldown/0.75);
            draw_list->AddCircleFilled(P0, 24.0, IM_COL32(209, 209, 255, C));
        }          
    }

    //
    // Building Menu
    //
    if (BuildingMode)
    {
        for (int i = 0; i < 30; i++)
        {
            for (int j = 0; j < 15; j++)
            {
                if (Grid[i][j] != 0)
                  continue;

                ImVec2 P0(1 + i * 42, 90 + j * 42);

                // Draw build button
                ImGui::SetCursorPos(P0);
                if (ImGui::Button(("##Build" + std::to_string(i) + "_" + std::to_string(j)).c_str(), ImVec2(40, 40)))
                {
                    //Build(i, j, BuildingMode);
                    Grid[i][j] = BuildingMode;
                    Turrets.push_back(Turret(i, j, BuildingMode));
                    PlayerCash -= ToSpend;
                    BuildingMode = 0;
                }
            }
        }
    }

    ImGui::End();
}

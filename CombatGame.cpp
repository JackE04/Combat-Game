#include <iostream>
#include <sstream>
#include <limits>

#include <string>

const char barChar = '#';

const int maxHealth = 100;
const int maxEnergy = 50;

std::string entityName;

int printStats(int entity[], bool isPlayer);

int main()
{
    int player[5] = { 100, 50, 0, 0, 0 }; //0 = health, 1 = energy, 2 = hashealed, 3 = hasdodged, 4 = hasrecharged
    int enemy[5] = { 100, 50, 0, 0, 0 };//0 = health, 1 = energy, 2 = hashealed, 3 = hasdodged, 4 = hasrecharged

    printStats(player, true);
    printStats(enemy, false);
}

int printStats(int entity[], bool isPlayer)
{
    if (isPlayer)
    {
        entityName = "Player";
    }
    else
    {
        entityName = "Enemy";
    }

    std::cout << entityName <<  " Health: [";

    int healthOutput = entity[0] / (maxHealth / 10); //Divide by 10 to make it fit the t10 block range for the bar output

    for (int i = 0; i < 10; i++)
    {
        if (i < healthOutput)
        {
            std::cout << barChar;
        }
        else
        {
            std::cout << " ";
        }
    }

    std::cout << "] " << entity[0] << " / " << maxHealth << "\n" << entityName << " Energy: [";

    int energyOutput = entity[1] / (maxEnergy / 10); //Divide by 10 to make it fit the t10 block range for the bar output

    for (int i = 0; i < 10; i++)
    {
        if (i < energyOutput)
        {
            std::cout << barChar;
        }
        else
        {
            std::cout << " ";
        }
    }

    std::cout << "] " << entity[1] << " / " << maxEnergy << "\n\n";

    return 0;
}
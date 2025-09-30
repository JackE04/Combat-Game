#include <iostream>
#include <sstream>
#include <limits>

#include <cstdlib>

#include <string>

const char barChar = '#';

const int maxHealth = 100;
const int maxEnergy = 50;
const int energyChargeRate = 4; //Amount of energy regained each turn

const int barLength = 10;

struct entity
{
    std::string name = "";
    int health = 100;
    int energy = 50;
    bool hasHealed = false;
    bool hasDodged = false;
    bool hasRecharged = false;
};

entity player;
entity enemy;

int moveNumber;
std::string playerChoice = "0";

void printStats(entity entity);

int playerInput();

void processMove(int moveNumber, entity *moveUser, entity *target);

int main()
{
    player.name = "Player";
    enemy.name = "Enemy";

    for (;;) //Initialise the loop of the turn
    {
        //Reset states for turn
        player.hasDodged = false;
        player.hasHealed = false;
        player.hasRecharged = false;
        enemy.hasDodged = false;
        enemy.hasHealed = false;
        enemy.hasRecharged = false;

        printStats(player);
        printStats(enemy);
        int moveNumber = playerInput();
        processMove(moveNumber, &player, &enemy);

        if (player.hasHealed) //Allow a second turn if the player has healed
        {
            printStats(player);
            printStats(enemy);
            int moveNumber = playerInput();
            processMove(moveNumber, &player, &enemy);
        }
    }

    return 0;
}

void printStats(entity entity)
{
    std::cout << entity.name <<  " Health: [";

    int healthOutput = entity.health / (maxHealth / barLength);

    for (int i = 0; i < 10; i++)
    {
        if (i < healthOutput)
        {
            std::wcout << barChar;
        }
        else
        {
            std::cout << " ";
        }
    }

    std::cout << "] " << entity.health << " / " << maxHealth << "\n" << entity.name << " Energy: [";

    int energyOutput = entity.energy / (maxEnergy / barLength);

    for (int i = 0; i < 10; i++)
    {
        if (i < energyOutput)
        {
            std::wcout << barChar;
        }
        else
        {
            std::cout << " ";
        }
    }

    std::cout << "] " << entity.energy << " / " << maxEnergy << "\n\n";
}

int playerInput()
{
    std::cout << "Choose your move:\n[1] Attack\n[2] Special Attack\n[3] Recharge\n[4] Dodge\n[5] Heal\n\nSelect a move by typing the number of the move: ";

    for (;;) //Initalise verification loop
    {
        std::cin.clear(); // Reset input error flag to allow new input

        std::cin >> playerChoice;

        for (char ch : playerChoice)
        {
            if (!std::isdigit(ch))
            {
                std::cin.setstate(std::ios_base::failbit);

                break;
            }
        }

        if (!std::cin.fail())
        {
            if (std::stoi(playerChoice) < 1 || std::stoi(playerChoice) > 5)
                //Make sure the previous check didn't set the cin fail state, if it didnt then we can safely do stoi on our integer string
            {
                std::cin.setstate(std::ios_base::failbit);
            }
            else
            {
                switch (std::stoi(playerChoice))
                {
                    case 1:
                    {
                        if (player.energy < 5)
                        {
                            std::cout << "Not enough energy! Try again: ";
                            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                            std::cin.setstate(std::ios_base::failbit);
                        }

                        break;
                    }
                    case 2:
                    {
                        if (player.energy < 10)
                        {
                            std::cout << "Not enough energy! Try again: ";
                            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                            std::cin.setstate(std::ios_base::failbit);
                        }

                        break;
                    }
                    case 5:
                    {
                        if (player.hasHealed)
                        {
                            std::cout << "You cannot heal more than once a turn!\nTry again: ";
                            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                            std::cin.setstate(std::ios_base::failbit);
                        }
                        else if (player.energy < 11)
                        {
                            std::cout << "Not enough energy! You must have atleast 1 energy left to convert to healing.\nTry again: ";
                            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                            std::cin.setstate(std::ios_base::failbit);
                        }
                        else if (player.health == maxHealth)
                        {
                            std::cout << "Health is full, cannot heal!\nTry again: ";
                            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                            std::cin.setstate(std::ios_base::failbit);
                        }

                        break;
                    }
                }
            }
        }

        if (!std::cin)
        {
            std::cout << "Invalid input. Try again: ";
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            // Clear input buffer, numeric limits allows us to get the size of the buffer to clear the whole thing
        }
        else
        {
            break;
        }
    }

    return std::stoi(playerChoice);
}

void processMove(int moveNumber, entity *moveUser, entity *target) 
//We're passing the entities as pointers so that we can update their values without needing to return the structs
{
    switch (moveNumber)
    {
        case 1: //Basic Attack
        {
            (*moveUser).energy -= 5;

            int chanceToHit = 80;

            int hitAttempt = rand() % 101;

            if ((*target).hasHealed)
            {
                chanceToHit += 20;
            }

            if ((*target).hasDodged)
            {
                chanceToHit -= 30;
            }

            if (hitAttempt <= hitAttempt)
            {
                int damage = rand() % 11 + 1;
                (*target).health -= damage;
            }

            break;
        }
        case 2: //Special Attack
        {
            (*moveUser).energy -= 10;

            int chanceToHit = 50;

            int hitAttempt = rand() % 101;

            if ((*target).hasHealed)
            {
                chanceToHit += 20;
            }

            if ((*target).hasDodged)
            {
                chanceToHit -= 30;
            }

            if (hitAttempt <= hitAttempt)
            {
                int damage = rand() % 21 + 5;
                (*target).health -= damage;
            }

            break;
        }
        case 3: //Recharge
        {
            (*moveUser).hasRecharged = true;

            break;
        }
        case 4: //Dodge
        {
            (*moveUser).hasDodged = true;

            break;
        }
        case 5: //Heal
        {
            (*moveUser).hasHealed = true;

            (*moveUser).energy -= 10;
            (*moveUser).health += (*moveUser).energy / 2;
            (*moveUser).energy /= 2;

            break;
        }
    }
}
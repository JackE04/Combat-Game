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

const int attackHitChance = 80;
const int specialAttackHitChance = 50;
const int dodgeChanceModifier = 30;
const int healChanceModifier = 20;

const int attackMinDamage = 1;
const int attackMaxDamage = 11; //10 + 1 for rand function range to process correctly

const int specialAttackMinDamage = 5;
const int specialAttackMaxDamage = 21; //20 + 1 for rand function range to process correctly

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

enum moveNumbers
{
    Attack = 1,
    SpecialAttack,
    Recharge,
    Dodge,
    Heal,
};

moveNumbers moveStates;

int moveNumber;
std::string playerChoice = "0";

int enemyChoice;
int enemyRoll;

//Initialise functions
void printStats(entity entity);
int playerInput();
void processMove(int moveNumber, entity &moveUser, entity &target);
void performAttackMove(int chanceToHit, entity& target, int minDamage, int maxDamage);
int enemyMoveChoice(entity &enemy);
void restoreEnergy(entity &entityToRestore);
void clearScreen();

int main()
{
    player.name = "Player";
    enemy.name = "Enemy";

    for (;;) //Initialise the loop of the turn
    {
        //Reset states for player turn
        player.hasDodged = false;
        player.hasHealed = false;
        player.hasRecharged = false;
        printStats(player);
        printStats(enemy);

        moveNumber = playerInput();
        processMove(moveNumber, player, enemy);

        if (player.hasHealed) //Allow a second turn if the player has healed
        {
            printStats(player);
            printStats(enemy);
            int moveNumber = playerInput();
            processMove(moveNumber, player, enemy);
        }

        clearScreen();
        printStats(player);
        printStats(enemy);
        std::cout << "Press enter to process enemy move!\n";
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        getchar(); //Wait for any user input to continue

        enemy.hasDodged = false;
        enemy.hasHealed = false;
        enemy.hasRecharged = false;

        moveNumber = enemyMoveChoice(enemy);
        processMove(moveNumber, enemy, player);

        if (enemy.health <= 0)
        {
            std::cout << "You have defeated the enemy, you win!";

            break;
        }
        else if(player.health <= 0)
        {
            std::cout << "You have been defeated by the enemy, you lose!";

            break;
        }
        else
        {
            //Restore energy per round
            restoreEnergy(player);
            restoreEnergy(enemy);
            clearScreen();
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

void processMove(int moveNumber, entity &moveUser, entity &target) 
//We're passing the entities as pointers so that we can update their values without needing to return the structs
{
    switch (moveNumber)
    {
        case Attack:
        {
            moveUser.energy -= 5;

            int chanceToHit = attackHitChance;

            performAttackMove(chanceToHit, target, attackMinDamage, attackMaxDamage);

            break;
        }
        case SpecialAttack:
        {
            moveUser.energy -= 10;

            int chanceToHit = specialAttackHitChance;

            performAttackMove(chanceToHit, target, specialAttackMinDamage, specialAttackMaxDamage);

            break;
        }
        case Recharge:
        {
            moveUser.hasRecharged = true;

            break;
        }
        case Dodge:
        {
            moveUser.hasDodged = true;

            break;
        }
        case Heal:
        {
            moveUser.hasHealed = true;

            moveUser.energy -= 10;
            moveUser.health += moveUser.energy / 2;
            moveUser.energy /= 2;

            break;
        }
    }
}

void performAttackMove(int chanceToHit, entity &target, int minDamage, int maxDamage)
{
    int hitAttempt = rand() % 101;

    if (target.hasHealed)
    {
        chanceToHit += healChanceModifier;
    }

    if (target.hasDodged)
    {
        chanceToHit -= dodgeChanceModifier;
    }

    if (hitAttempt <= hitAttempt)
    {
        int damage = rand() % (maxDamage - minDamage) + minDamage;
        target.health -= damage;
    }
}

int enemyMoveChoice(entity &enemy)
{
    if (enemy.energy <= 10)
    {
        enemyChoice = 3; //Set enemy to recharge
    }
    else if (enemy.health <= 20)
    {
        //Rnadomly decide to heal or dodge on low health
        enemyRoll = rand() % 2;

        switch (enemyRoll)
        {
            case 0:
            {
                enemyChoice = 5;

                break;
            }
            case 1:
            {
                enemyChoice = 4;

                break;
            }
        }
    }
    else if (enemy.energy >= 10)
    {
        enemyRoll = rand() % 2;

        switch (enemyRoll)
        {
            case 0:
            {
                enemyChoice = 2;

                break;
            }
            case 1:
            {
                enemyChoice = 1;

                break;
            }
        }
    }
    else
    {
        enemyChoice = 1;
    }

    return enemyChoice;
}

void restoreEnergy(entity &entityToRestore)
{
    if (entityToRestore.hasRecharged)
    {
        entityToRestore.energy += 16;
    }
    else
    {
        entityToRestore.energy += 4;
    }
}

void clearScreen()
{
    std::cout << "\033[2J\033[1;1H";
}
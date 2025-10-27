#include <iostream>
#include <sstream>
#include <limits>

#include <cstdlib>

#include <string>

#include <Windows.h>

const char barChar = '#';
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

const int maxHealth = 100;
const int maxEnergy = 50;
const int energyRegainRate = 4;

enum moveNumbers
{
    Attack = 1,
    SpecialAttack,
    Recharge,
    Dodge,
    Heal,
};

const int attackHitChance = 80;
const int attackMinDamage = 1;
const int attackMaxDamage = 10;
const int attackMinEnergy = 5;

const int specialAttackHitChance = 50;
const int specialAttackMinDamage = 5;
const int specialAttackMaxDamage = 20;
const int specialAttackMinEnergy = 10;

const int dodgeChanceModifier = 30;

const int healChanceModifier = 20;
const int healMinEnergy = 10;

int moveNumber;
std::string playerChoice = "1";
const int lowestPlayerChoice = 1;
const int highestPlayerChoice = 5;
std::string moveString = "";

int enemyChoice;
int enemyRoll;

//Initialise functions
void printRoundProgress(std::string printContinue, std::string printMove);
void clearScreen();
void printStats(entity entity);
int playerInput();
void setCinFail();
std::string processMove(int moveNumber, entity &moveUser, entity &target);
int performAttackMove(int chanceToHit, entity& target, int minDamage, int maxDamage);
int enemyMoveChoice(entity &enemy);
void restoreEnergy(entity &entityToRestore);

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
        
        printRoundProgress("","");

        moveNumber = playerInput();
        moveString = processMove(moveNumber, player, enemy);

        if (player.hasHealed) //Allow a second turn if the player has healed
        {
            printRoundProgress("", moveString);

            int moveNumber = playerInput();
            moveString = processMove(moveNumber, player, enemy);
        }

        printRoundProgress("Press enter to process enemy move!\n", moveString);

        enemy.hasDodged = false;
        enemy.hasHealed = false;
        enemy.hasRecharged = false;

        moveNumber = enemyMoveChoice(enemy);
        moveString = processMove(moveNumber, enemy, player);

        if (enemy.hasHealed) //Allow a second turn if the enemy has healed
        {
            moveNumber = enemyMoveChoice(enemy);
            moveString = processMove(moveNumber, enemy, player);
        }

        printRoundProgress("Press enter to end round!\n", moveString);

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
        }
    }

    return 0;
}

void printRoundProgress(std::string printContinue, std::string printMove = "")
{
    clearScreen();
    printStats(player);
    printStats(enemy);

    if (printMove != "")
    {
        std::cout << printMove;
    }

    if (printContinue != "")
    {
        std::cout << printContinue;
        std::cin.clear();
        std::cin.ignore((std::numeric_limits<std::streamsize>::max)(), '\n');
        std::ignore = getchar(); //Wait for any user input to continue
    }
}

void clearScreen() //This function has been taken from the windows example console clear page, Example 2: https://learn.microsoft.com/en-us/windows/console/clearing-the-screen 
{
    HANDLE hStdout = GetStdHandle(STD_OUTPUT_HANDLE);

    CONSOLE_SCREEN_BUFFER_INFO csbi;
    SMALL_RECT scrollRect;
    COORD scrollTarget;
    CHAR_INFO fill;

    // Get the number of character cells in the current buffer.
    if (!GetConsoleScreenBufferInfo(hStdout, &csbi))
    {
        return;
    }

    // Scroll the rectangle of the entire buffer.
    scrollRect.Left = 0;
    scrollRect.Top = 0;
    scrollRect.Right = csbi.dwSize.X;
    scrollRect.Bottom = csbi.dwSize.Y;

    // Scroll it upwards off the top of the buffer with a magnitude of the entire height.
    scrollTarget.X = 0;
    scrollTarget.Y = (SHORT)(0 - csbi.dwSize.Y);

    // Fill with empty spaces with the buffer's default text attribute.
    fill.Char.UnicodeChar = TEXT(' ');
    fill.Attributes = csbi.wAttributes;

    // Do the scroll
    ScrollConsoleScreenBuffer(hStdout, &scrollRect, NULL, scrollTarget, &fill);

    // Move the cursor to the top left corner too.
    csbi.dwCursorPosition.X = 0;
    csbi.dwCursorPosition.Y = 0;

    SetConsoleCursorPosition(hStdout, csbi.dwCursorPosition);
}

void printStats(entity entity)
{
    std::cout << entity.name <<  " Health: [";

    int healthOutput = entity.health / (maxHealth / barLength);

    for (int i = 0; i < barLength; i++)
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

    for (int i = 0; i < barLength; i++)
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
            if (std::stoi(playerChoice) < lowestPlayerChoice || std::stoi(playerChoice) > highestPlayerChoice)
                //Make sure the previous check didn't set the cin fail state, if it didnt then we can safely do stoi on our integer string
            {
                std::cin.setstate(std::ios_base::failbit);
            }
            else
            {
                switch (std::stoi(playerChoice))
                {
                    case Attack:
                    {
                        if (player.energy < attackMinEnergy)
                        {
                            std::cout << "Not enough energy! Try again: ";
                            setCinFail();
                        }

                        break;
                    }
                    case SpecialAttack:
                    {
                        if (player.energy < specialAttackMinEnergy)
                        {
                            std::cout << "Not enough energy! Try again: ";
                            setCinFail();
                        }

                        break;
                    }
                    case Recharge:
                    {
                        if (player.energy == maxEnergy)
                        {
                            std::cout << "You cannot recharge over the max energy!\nTry again: ";
                            setCinFail();
                        }
                        
                        break;
                    }
                    case Heal:
                    {
                        if (player.hasHealed)
                        {
                            std::cout << "You cannot heal more than once a turn!\nTry again: ";
                            setCinFail();
                        }
                        else if (player.energy < (healMinEnergy + 1)) //Add 1 to the minimum energy to make sure we have atleast 1 energy to convert to healing after the cost of the move
                        {
                            std::cout << "Not enough energy! You must have atleast 1 energy left to convert to healing.\nTry again: ";
                            setCinFail();
                        }
                        else if (player.health == maxHealth)
                        {
                            std::cout << "Health is full, cannot heal!\nTry again: ";
                            setCinFail();
                        }

                        break;
                    }
                }
            }
        }

        if (!std::cin)
        {
            std::cout << "Invalid input. Try again: ";
            std::cin.ignore((std::numeric_limits<std::streamsize>::max)(), '\n');
            // Clear input buffer, numeric limits allows us to get the size of the buffer to clear the whole thing
        }
        else
        {
            break;
        }
    }

    return std::stoi(playerChoice);
}

void setCinFail()
{
    std::cin.ignore((std::numeric_limits<std::streamsize>::max)(), '\n');
    std::cin.setstate(std::ios_base::failbit);
}

std::string processMove(int moveNumber, entity &moveUser, entity &target) 
//We're passing the entities as pointers so that we can update their values without needing to return the structs
{
    switch (moveNumber)
    {
        case Attack:
        {
            moveUser.energy -= attackMinEnergy;

            int chanceToHit = attackHitChance;

            int damageResult = performAttackMove(chanceToHit, target, attackMinDamage, attackMaxDamage);
             
            if (damageResult == 0)
            {
                moveString = "\nAttack Missed!\n\n";
            }
            else
            {
                moveString = "\n" + moveUser.name + " used Attack dealing " + std::to_string(damageResult) + " damage!\n\n";
            }

            break;
        }
        case SpecialAttack:
        {
            moveUser.energy -= specialAttackMinEnergy;

            int chanceToHit = specialAttackHitChance;

            int damageResult = performAttackMove(chanceToHit, target, specialAttackMinDamage, specialAttackMaxDamage);

            if (damageResult == 0)
            {
                moveString = "\nAttack Missed!\n\n";
            }
            else
            {
                moveString = "\n" + moveUser.name + " used Special Attack dealing " + std::to_string(damageResult) + " damage!\n\n";
            }

            break;
        }
        case Recharge:
        {
            moveUser.hasRecharged = true;

            moveString = "\n" + moveUser.name + " used Recharge!\n\n";

            break;
        }
        case Dodge:
        {
            moveUser.hasDodged = true;

            moveString = "\n" + moveUser.name + " used Dodge!\n\n";

            break;
        }
        case Heal:
        {
            moveUser.hasHealed = true;

            moveUser.energy -= healMinEnergy;
            
            int healthRegained = moveUser.energy / 2; //Convert half of energy to health
            moveUser.health += healthRegained;

            moveUser.energy /= 2; //Used half of energy to heal

            moveString = "\n" + moveUser.name + " used Heal recovering " + std::to_string(healthRegained) + " health costing " + std::to_string(moveUser.energy) + " energy!\n\n";

            break;
        }
    }

    return moveString;
}

int performAttackMove(int chanceToHit, entity &target, int minDamage, int maxDamage)
{
    int hitAttempt = rand() % 101; // 1 in 100

    if (target.hasHealed)
    {
        chanceToHit += healChanceModifier;
    }

    if (target.hasDodged)
    {
        chanceToHit -= dodgeChanceModifier;
    }

    if (hitAttempt <= chanceToHit)
    {
        int damage = rand() % ((maxDamage + 1) - minDamage) + minDamage; // +1 to maxDamage for rand function range to process correctly
        target.health -= damage;

        return damage;
    }
    else
    {
        return 0;
    }
}

int enemyMoveChoice(entity &enemy)
{
    if (enemy.energy <= specialAttackMinEnergy)
    {
        enemyChoice = Recharge;
    }
    else if (enemy.health <= maxHealth / 5)
    {
        if (!enemy.hasHealed)
        {
            enemyRoll = rand() % 2; // 1 in 2

            switch (enemyRoll)
            {
                case 0:
                {
                    enemyChoice = Heal;

                    break;
                }
                case 1:
                {
                    enemyChoice = Dodge;

                    break;
                }
            }
        }
        else
        {
            enemyChoice = Dodge;
        }
    }
    else if (enemy.energy >= specialAttackMinEnergy)
    {
        enemyRoll = rand() % 2; // 1 in 2

        switch (enemyRoll)
        {
            case 0:
            {
                enemyChoice = SpecialAttack;

                break;
            }
            case 1:
            {
                enemyChoice = Attack;

                break;
            }
        }
    }
    else
    {
        enemyChoice = Attack;
    }

    return enemyChoice;
}

void restoreEnergy(entity &entityToRestore)
{
    int energyToRestore = energyRegainRate;

    if (entityToRestore.hasRecharged)
    {
        energyToRestore *= energyRegainRate; //Restore energy at 4 times the normal rate when recharging aka same as energyRegainRate
    }

    if (entityToRestore.hasDodged)
    {
        energyToRestore /= 2; //Dodging halves energy restore rate this turn
    }

    entityToRestore.energy += energyToRestore;
}
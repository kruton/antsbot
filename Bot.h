#ifndef BOT_H_
#define BOT_H_

#include <map>

#include "State.h"
#include "QAgent.h"

typedef std::map<Location, Location> Commands;

/*
    This struct represents your bot in the game of Ants
*/
struct Bot
{
    QAgent agent;
    State state;
    Commands mCommands;

    Bot();
    ~Bot();

    float checkReward(const Map& oldMap, const Map& map, const Location& oldLoc,
            const Location& newLoc) const;

    void playGame();    //plays a single game of Ants

    void makeMoves();   //makes moves for a single turn
    void endTurn();     //indicates to the engine that it has made its moves
};

#endif //BOT_H_

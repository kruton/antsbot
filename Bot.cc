#include <unistd.h>
#include "Bot.h"

using namespace std;

//constructor
Bot::Bot()
        : mCommands()
{
    agent.readWeights("weights.txt");
}

Bot::~Bot() {
    agent.writeWeights("weights.txt");
}

//plays a single game of Ants.
void Bot::playGame() {
    //reads the game parameters and sets up
    cin >> state;
    state.setup();
    endTurn();

    //continues making moves while the game is not over
    while (cin >> state) {
        state.updateVisionInformation();
        makeMoves();
        endTurn();
    }
}

float Bot::checkReward(const Map& oldMap, const Map& map,
        const Location& oldLoc, const Location& loc) const {
    float reward = 0.0f;

    if (!oldMap.isExplored(loc)) {
        cerr << "giving reward for exploring" << endl;
        reward += 0.2f;
    }

    for (int d = 0; d < TDIRECTIONS; d++) {
        const Location newLoc = oldMap.getLocation(loc, d);
        if (oldMap.isFood(newLoc) && !map.isFood(newLoc)) {
            cerr << "giving reward to " << loc << " for eating" << endl;
            return reward + 10.0f;
        }
    }

    if (!map.isMyAnt(loc)) {
        cerr << "giving penalty to " << loc << " for dying" << endl;

        return reward - 15.0f;
    }

    if (map.isOnlyDeadEnemyAnts(loc)) {
        cerr << "giving reward to " << loc << " for killing" << endl;

        return reward + 10.0f;
    }

    if (oldMap.isEnemyHill(loc)) {
        cerr << "giving reward to " << loc << " for razing" << endl;

        return reward + 100.0f;
    }

    return reward;
}

//makes the bots moves for the turn
void Bot::makeMoves() {
    state.bug << "turn " << state.turn << ":" << endl;
    state.bug << state << endl;

    const Locations myAnts = state.getMyAnts();
    Commands::const_iterator cit;
    for (cit = mCommands.begin(); cit != mCommands.end(); cit++) {
        const Location oldLoc = (*cit).first;
        const Location newLoc = (*cit).second;

        const Map& oldMap = *state.getOldMap();
        const Map& map = *state.getMap();

        const float reward = checkReward(oldMap, map, oldLoc, newLoc);

        agent.update(state, oldMap, map, oldLoc, newLoc, reward);
    }

    mCommands.clear();

    Locations::const_iterator it;
    for (it = myAnts.begin(); it != myAnts.end(); it++) {
        const Location loc = *it;
        const Location dest = agent.getAction(state, *state.getMap(), loc);
        state.makeMove(loc, dest);
        mCommands[loc] = dest;
    }

    state.bug << "time taken: " << state.timer.getTime() << "ms" << endl
            << endl;
}

//finishes the turn
void Bot::endTurn() {
    if (state.turn > 0)
        state.reset();
    state.turn++;

    cout << "go" << endl;
}

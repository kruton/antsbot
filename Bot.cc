#include "Bot.h"
#include "micropather.h"

using namespace std;

//constructor
Bot::Bot() {

}
;

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
;

//makes the bots moves for the turn
void Bot::makeMoves() {
    state.bug << "turn " << state.turn << ":" << endl;
    state.bug << state << endl;

    for (int food = 0; food < (int) state.food.size(); food++) {
        const Location foodLoc = state.food[food];

        const Location* closestAnt = NULL;
        unsigned long closestDistance = 0UL;

        for (int ant = 0; ant < (int) state.myAnts.size(); ant++) {
            const Location antLoc = state.myAnts[ant];

            const unsigned long antDistance = state.manhattanDistance(foodLoc, antLoc);
            if (closestAnt == NULL || antDistance < closestDistance) {
                closestAnt = &antLoc;
                closestDistance = antDistance;
            }
        }

        if (closestAnt != NULL) {
            micropather::MicroPather pather(&state);
            std::vector<void*> path;
            float totalCost;

            int result = pather.Solve((void*) closestAnt, (void*) &foodLoc, &path, &totalCost);
            const Location* next = (const Location*) path.front();
            state.makeMove(*closestAnt, state.getDirection(*closestAnt, *next));
            vector<Location>::iterator it = find(state.myAnts.begin(), state.myAnts.end(), *closestAnt);
            state.myAnts.erase(it);
        }
    }

    //picks out moves for each ant
    for (int ant = 0; ant < (int) state.myAnts.size(); ant++) {
        for (int d = 0; d < TDIRECTIONS; d++) {
            Location loc = state.getLocation(state.myAnts[ant], d);

            if (!state.grid[loc.row][loc.col].isWater && !state.grid[loc.row][loc.col].isHill) {
                state.makeMove(state.myAnts[ant], d);
                break;
            }
        }
    }

    state.bug << "time taken: " << state.timer.getTime() << "ms" << endl
            << endl;
}
;

//finishes the turn
void Bot::endTurn() {
    if (state.turn > 0)
        state.reset();
    state.turn++;

    cout << "go" << endl;
}
;

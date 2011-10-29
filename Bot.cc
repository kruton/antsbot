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

    for (int food = 0; food < (int) state.food.size() && (int) state.myAnts.size() > 0; food++) {
        const Location foodLoc = state.food[food];

        vector<void*>* closestPath = NULL;
        unsigned long closestDistance = 0UL;

        for (int ant = 0; ant < (int) state.myAnts.size(); ant++) {
            const Location antLoc = state.myAnts[ant];

            const unsigned long antDistance = state.manhattanDistance(foodLoc, antLoc);
            if (antDistance == 0) {
                break;
            }

            if (closestPath == NULL || antDistance < closestDistance) {
                micropather::MicroPather pather(&state);
                std::vector<void*>* path = new vector<void*>();
                float totalCost;

                int result = pather.Solve(antLoc.toState(), foodLoc.toState(),
                        path, &totalCost);
                if (result != micropather::MicroPather::NO_SOLUTION) {
                    closestDistance = totalCost;
                    if (closestPath != NULL) {
//                        delete closestPath;
                    }
                    closestPath = path;
                } else {
//                    delete path;
                }
            }

        }

        if (closestPath != NULL) {
            const Location ant = Location(closestPath->at(0));
            const Location next = Location(closestPath->at(1));

            int direction = state.getDirection(ant, next);
            if (direction != -1) {
                state.makeMove(ant, state.getDirection(ant, next));
                vector<Location>::iterator it = find(state.myAnts.begin(),
                        state.myAnts.end(), ant);
                state.myAnts.erase(it);
            } else {
                cerr << "wtf path from " << ant << " to " << foodLoc << ": ";
                vector<void*>::iterator it = (*closestPath).begin();
                while (it != (*closestPath).end()) {
                    cerr << Location(*it) << ' ';
                    it++;
                }
                cerr << endl;
            }
//                delete closestPath;
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

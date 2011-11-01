#include "State.h"

using namespace std;

//constructor
State::State()
        : mOldMap(NULL)
        , mMap(NULL)
{
    gameover = 0;
    turn = 0;
    bug.open("./debug.txt");
}

//deconstructor
State::~State() {
    if (mOldMap != NULL) {
        delete mOldMap;
    }
    if (mMap != NULL) {
        delete mMap;
    }
    bug.close();
}

//sets the state up
void State::setup() {
}

//resets all non-water squares to land and clears the bots ant vector
void State::reset() {
    if (mOldMap != NULL) {
        delete mOldMap;
    }

    mOldMap = mMap;
    mMap = new Map(mOldMap);
}

//outputs move information to the engine
void State::makeMove(const Location &loc, const Location& dest) {
    const int direction = mMap->getDirection(loc, dest);

    cout << "o " << loc.row << " " << loc.col << " " << CDIRECTIONS[direction]
            << endl;

    mMap->moveAnt(loc, direction);
}

Locations State::getLegalMoves(const Location& loc) const {
    return mMap->getLegalMoves(loc);
}

const Locations State::getMyAnts() const {
    return mMap->getMyAnts();
}

/*
 This function will update update the lastSeen value for any squares currently
 visible by one of your live ants.

 BE VERY CAREFUL IF YOU ARE GOING TO TRY AND MAKE THIS FUNCTION MORE EFFICIENT,
 THE OBVIOUS WAY OF TRYING TO IMPROVE IT BREAKS USING THE EUCLIDEAN METRIC, FOR
 A CORRECT MORE EFFICIENT IMPLEMENTATION, TAKE A LOOK AT THE GET_VISION FUNCTION
 IN ANTS.PY ON THE CONTESTS GITHUB PAGE.
 */
void State::updateVisionInformation() {
    mMap->updateVisionInformation(*this);
}

const Map* State::getMap() const {
    return mMap;
}

const Map* State::getOldMap() const {
    return mOldMap;
}

/*
 This is the output function for a state. It will add a char map
 representation of the state to the output stream passed to it.

 For example, you might call "cout << state << endl;"
 */
ostream& operator<<(ostream &os, const State& state) {
    os << *(state.mMap);

    return os;
}
;

//input function
istream& operator>>(istream &is, State& state) {
    int row, col, player;
    string inputType, junk;

    //finds out which turn it is
    while (is >> inputType) {
        if (inputType == "end") {
            state.gameover = 1;
            break;
        } else if (inputType == "turn") {
            is >> state.turn;
            break;
        } else
            //unknown line
            getline(is, junk);
    }

    if (state.turn == 0) {
        int rows, cols;

        //reads game parameters
        while (is >> inputType) {
            if (inputType == "loadtime")
                is >> state.loadtime;
            else if (inputType == "turntime")
                is >> state.turntime;
            else if (inputType == "rows")
                is >> rows;
            else if (inputType == "cols")
                is >> cols;
            else if (inputType == "turns")
                is >> state.turns;
            else if (inputType == "player_seed")
                is >> state.seed;
            else if (inputType == "viewradius2") {
                is >> state.viewradius;
                state.viewradius = sqrt(state.viewradius);
            } else if (inputType == "attackradius2") {
                is >> state.attackradius;
                state.attackradius = sqrt(state.attackradius);
            } else if (inputType == "spawnradius2") {
                is >> state.spawnradius;
                state.spawnradius = sqrt(state.spawnradius);
            } else if (inputType == "ready") //end of parameter input
                    {
                state.timer.start();
                break;
            } else
                //unknown line
                getline(is, junk);
        }

        state.mMap = new Map(rows, cols);
    } else {
        //reads information about the current turn
        while (is >> inputType) {
            if (inputType == "w") //water square
                    {
                is >> row >> col;
                state.mMap->addWater(row, col);
            } else if (inputType == "f") //food square
                    {
                is >> row >> col;
                state.mMap->addFood(row, col);
            } else if (inputType == "a") //live ant square
                    {
                is >> row >> col >> player;
                state.mMap->addAnt(row, col, player);
            } else if (inputType == "d") //dead ant square
                    {
                is >> row >> col >> player;
                state.mMap->addDeadAnt(row, col, player);
            } else if (inputType == "h") {
                is >> row >> col >> player;
                state.mMap->addHill(row, col, player);
            } else if (inputType == "players") //player information
                is >> state.noPlayers;
            else if (inputType == "scores") //score information
                    {
                state.scores = vector<double>(state.noPlayers, 0.0);
                for (int p = 0; p < state.noPlayers; p++)
                    is >> state.scores[p];
            } else if (inputType == "go") //end of turn input
                    {
                if (state.gameover)
                    is.setstate(std::ios::failbit);
                else
                    state.timer.start();
                break;
            } else
                //unknown line
                getline(is, junk);
        }
    }

    return is;
}

std::ostream& operator<<(std::ostream &os, const Location &loc) {
    os << '(' << loc.col << ',' << loc.row << ')';
    return os;
}

bool operator<(const Location& loc1, const Location& loc2) {
    return loc1.row < loc2.row || loc1.col < loc2.col;
}

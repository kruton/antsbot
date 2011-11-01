#ifndef STATE_H_
#define STATE_H_

#include <iostream>
#include <stdio.h>
#include <cstdlib>
#include <cmath>
#include <string>
#include <vector>
#include <queue>
#include <stack>
#include <map>
#include <stdint.h>

#include "Timer.h"
#include "Bug.h"
#include "Square.h"
#include "Location.h"
#include "Feature.h"
#include "Map.h"

/*
    constants
*/
const int TDIRECTIONS = 4;
const char CDIRECTIONS[4] = {'N', 'E', 'S', 'W'};

const int DIRECTIONS[4][2] = { {-1, 0}, {0, 1}, {1, 0}, {0, -1} };      //{N, E, S, W}

/*
    struct to store current state information
*/
class State {
public:
    /*
        Variables
    */
    int turn, turns,
        noPlayers;
    double attackradius, spawnradius, viewradius;
    double loadtime, turntime;
    std::vector<double> scores;
    bool gameover;
    int64_t seed;

    Map* mOldMap;
    Map* mMap;

    Timer timer;
    Bug bug;

    /*
        Functions
    */
    State();
    ~State();

    void setup();
    void reset();

    void makeMove(const Location& loc, const Location& dest);

    const Locations getMyAnts() const;

    Locations getLegalMoves(const Location& loc) const;

    void updateVisionInformation();

    const Map* getMap() const;

    const Map* getOldMap() const;
};

std::istream& operator>>(std::istream& is, State& state);
std::ostream& operator<<(std::ostream& os, const State& state);

#endif //STATE_H_

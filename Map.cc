/*
 * Map.cc
 *
 *  Created on: Nov 1, 2011
 *      Author: kenny
 */

#include <queue>
#include <vector>
#include <set>

#include "State.h"
#include "Map.h"

using namespace std;

Map::Map(int rows, int cols) :
        mRows(rows), mCols(cols), mGrid(rows, GridRow(cols, Square())) {
    mMaxDistance = mRows + mCols;
}

Map::Map(const Map* orig)
        : mRows(orig->mRows)
        , mCols(orig->mCols)
        , mMaxDistance(orig->mMaxDistance)
        , mGrid(mRows, GridRow(mCols, Square())) {
    Locations::const_iterator it;
    for (it = orig->mWater.begin(); it != orig->mWater.end(); it++) {
        mWater.push_back(*it);
        mGrid[(*it).row][(*it).col].isWater = true;
    }
}

Map::~Map() {
}

void Map::updateVisionInformation(const State& state) {
    std::queue<Location> locQueue;
    Location sLoc, cLoc, nLoc;

    for (int a = 0; a < (int) mMyAnts.size(); a++) {
        sLoc = mMyAnts[a];
        locQueue.push(sLoc);

        std::vector<std::vector<bool> > visited(mRows,
                std::vector<bool>(mCols, 0));
        mGrid[sLoc.row][sLoc.col].isVisible = 1;
        visited[sLoc.row][sLoc.col] = 1;

        while (!locQueue.empty()) {
            cLoc = locQueue.front();
            locQueue.pop();

            for (int d = 0; d < TDIRECTIONS; d++) {
                nLoc = getLocation(cLoc, d);

                if (!visited[nLoc.row][nLoc.col]
                        && distance(sLoc, nLoc) <= state.viewradius) {
                    mGrid[nLoc.row][nLoc.col].isVisible = 1;
                    locQueue.push(nLoc);
                }
                visited[nLoc.row][nLoc.col] = 1;
            }
        }
    }
}

//returns the new location from moving in a given direction with the edges wrapped
Location Map::getLocation(const Location &loc, int direction) const {
    return Location((loc.row + DIRECTIONS[direction][0] + mRows) % mRows,
            (loc.col + DIRECTIONS[direction][1] + mCols) % mCols);
}
;

void Map::addHill(int row, int col, int player) {
    mGrid[row][col].isHill = 1;
    mGrid[row][col].hillPlayer = player;

    if (player == 0) {
        mMyHills.push_back(Location(row, col));
    } else {
        mEnemyHills.push_back(Location(row, col));
    }
}

void Map::addDeadAnt(int row, int col, int player) {
    mGrid[row][col].deadAnts.push_back(player);
}

void Map::addAnt(int row, int col, int player) {
    mGrid[row][col].ant = player;

    if (player == 0) {
        mMyAnts.push_back(Location(row, col));
    } else {
        mEnemyAnts.push_back(Location(row, col));
    }
}

void Map::addFood(int row, int col) {
    mGrid[row][col].isFood = 1;
    mFood.push_back(Location(row, col));
}

void Map::addWater(int row, int col) {
    mGrid[row][col].isWater = 1;
    mWater.push_back(Location(row, col));
}

void Map::moveAnt(const Location& loc, int direction) {
    Location nLoc = getLocation(loc, direction);
    mGrid[nLoc.row][nLoc.col].ant = mGrid[loc.row][loc.col].ant;
    mGrid[loc.row][loc.col].ant = -1;
}

bool Map::isEnemyHill(const Location& loc) const {
    return mGrid[loc.row][loc.col].isHill
            && mGrid[loc.row][loc.col].hillPlayer != 0;
}

bool Map::isMyHill(const Location& loc) const {
    return mGrid[loc.row][loc.col].isHill
            && mGrid[loc.row][loc.col].hillPlayer == 0;
}

bool Map::isFood(const Location& loc) const {
    return mGrid[loc.row][loc.col].isFood;
}

bool Map::isOnlyDeadEnemyAnts(const Location& loc) const {
    vector<int> deadAnts = mGrid[loc.row][loc.col].deadAnts;

    bool selfDied = false;

    vector<int>::const_iterator it;
    for (it = deadAnts.begin(); it != deadAnts.end(); it++) {
        if (*it == 0) {
            selfDied = true;
            break;
        }
    }

    return !selfDied && deadAnts.size() > 0;
}

bool Map::isMyAnt(const Location& loc) const {
    return mGrid[loc.row][loc.col].ant == 0;
}

Features Map::extractFeatures(const State& state, const Location& pos) const {
    Features features;

    features[Bias] = 0.1f;

    if (isEnemyHill(pos)) {
        features[RazesEnemyHill] = 0.2f;
    }

    const int32_t antsNear = countAntsNextTo(pos);
    if (antsNear > 0) {
        features[HaveBackup] = (float) (antsNear > 4 ? 4.0f : (float) antsNear) / 16.0f;
    }

    int32_t closest;
    const uint32_t enemyAntsInSight = countEnemyAntsInSight(pos, state.viewradius, &closest);
    float danger = 0.0f;
    if (enemyAntsInSight > 0) {
        danger = (closest - state.attackradius) / state.viewradius;
        features[EnemyNear] = max(1.0f, danger);

        if (enemyAntsInSight > antsNear) {
            features[Outnumbered] = 0.1f;
        }
    }

    // Only think of food if we're not in immediate danger.
    if (danger < 0.25f) {
        bool foodNear = false;
        if (isFood(pos)) {
            foodNear = true;
        } else {
            for (int d = 0; d < TDIRECTIONS; d++) {
                const Location posNeighbor = getLocation(pos, d);
                if (isFood(posNeighbor)) {
                    foodNear = true;
                    break;
                }
            }
        }

        if (foodNear) {
            features[EatsFood] = 0.1f;
        }

        const float foodDistance = getClosestFoodDistance(pos);
        if (foodDistance > 0) {
            features[DistanceToFood] = (float) foodDistance / (float) getMaxDistance();
        }

    }

    return features;
}

const Locations Map::getMyAnts() const {
    return mMyAnts;
}

int32_t Map::getMaxDistance() const {
    return mMaxDistance;
}

int32_t Map::getClosestFoodDistance(const Location& loc) const {
    int32_t closest = mMaxDistance + 1;

    Locations::const_iterator it;
    for (it = mFood.begin(); it != mFood.end(); it++) {
        closest = min(closest, (int32_t) manhattanDistance(loc, *it));
    }

    return closest > mMaxDistance ? -1 : closest;
}

uint32_t Map::countAntsNextTo(const Location& loc) const {
    stack<Location> frontier;
    set<Location> visited;

    for (int i = 0; i < TDIRECTIONS; i++) {
        const Location newLoc = getLocation(loc, i);
        frontier.push(newLoc);
        visited.insert(loc);
    }

    int numAnts = 0;
    while (!frontier.empty()) {
        const Location l = frontier.top();
        frontier.pop();

        if (mGrid[l.row][l.col].ant == 0) {
            numAnts++;

            for (int i = 0; i < TDIRECTIONS; i++) {
                const Location newLoc = getLocation(l, i);
                if (visited.find(newLoc) == visited.end()) {
                    frontier.push(newLoc);
                    visited.insert(l);
                }
            }
        }
    }

    // Don't count ourselves.
    return numAnts;
}

uint32_t Map::countEnemyAntsInSight(const Location& loc, uint32_t range, int32_t* closest) const {
    Locations::const_iterator it;

    int32_t closestAnt = -1;
    uint32_t numAnts = 0;

    for (it = mEnemyAnts.begin(); it != mEnemyAnts.end(); it++) {
        int32_t distance = manhattanDistance(loc, *it);
        if (distance <= range) {
            if (closestAnt == -1 || distance < closestAnt) {
                closestAnt = distance;
            }
            numAnts++;
        }
    }

    if (closest != NULL) {
        *closest = closestAnt;
    }

    return numAnts;
}

uint32_t Map::manhattanDistance(const Location &loc1,
        const Location &loc2) const {
    int d1 = abs(loc1.row - loc2.row), d2 = abs(loc1.col - loc2.col), dr = min(
            d1, mRows - d1), dc = min(d2, mCols - d2);
    return dr + dc;
}

//returns the euclidean distance between two locations with the edges wrapped
double Map::distance(const Location &loc1, const Location &loc2) const {
    int d1 = abs(loc1.row - loc2.row), d2 = abs(loc1.col - loc2.col), dr = min(
            d1, mRows - d1), dc = min(d2, mCols - d2);
    return sqrt(dr * dr + dc * dc);
}
;

int Map::getDirection(const Location& start, const Location& end) const {
    for (int direction = 0; direction < TDIRECTIONS; direction++) {
        const int row = (start.row + DIRECTIONS[direction][0] + mRows) % mRows;
        const int col = (start.col + DIRECTIONS[direction][1] + mCols) % mCols;
        if (end.row == row && end.col == col) {
            return direction;
        }
    }

    cerr << "Invalid move from " << start << " to " << end << endl;
    return -1;
}

Locations Map::getLegalMoves(const Location& pos) const {
    Locations adjacent;

    for (int d = 0; d < TDIRECTIONS; d++) {
        const Location next = getLocation(pos, d);

        const Square& nextSquare = mGrid[next.row][next.col];
        if (!nextSquare.isWater && nextSquare.ant != 0) {
            adjacent.push_back(next);
        }
    }

    return adjacent;
}

ostream& operator<<(ostream& os, const Map& map) {
    for (int row = 0; row < map.mRows; row++) {
        for (int col = 0; col < map.mCols; col++) {
            if (map.mGrid[row][col].isWater)
                os << '%';
            else if (map.mGrid[row][col].isFood)
                os << '*';
            else if (map.mGrid[row][col].ant >= 0)
                os << (char) ('a' + map.mGrid[row][col].ant);
            else if (map.mGrid[row][col].isHill)
                os << (char) ('A' + map.mGrid[row][col].hillPlayer);
            else if (map.mGrid[row][col].isVisible)
                os << '.';
            else
                os << '?';
        }
        os << endl;
    }

    return os;
}

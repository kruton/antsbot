/*
 * Map.h
 *
 *  Created on: Nov 1, 2011
 *      Author: kenny
 */

#include <iostream>

#include "Location.h"
#include "Square.h"

#ifndef MAP_H_
#define MAP_H_

typedef std::vector<Square> GridRow;
typedef std::vector<GridRow> Grid;

// Forward declaration
class State;

class Map {
    friend std::ostream& operator<<(std::ostream& os, const Map& map);

public:
    Map(int rows, int cols);
    Map(const Map* orig);
    virtual ~Map();

    void updateVisionInformation(const State& state);

    void addHill(int row, int col, int player);

    void addDeadAnt(int row, int col, int player);

    void addAnt(int row, int col, int player);

    void addFood(int row, int col);

    void addWater(int row, int col);

    bool isEnemyHill(const Location& loc) const;

    bool isMyHill(const Location& loc) const;

    bool isFood(const Location& loc) const;

    bool isMyAnt(const Location& loc) const;

    bool isOnlyDeadEnemyAnts(const Location& loc) const;

    Features extractFeatures(const State& state, const Location& loc) const;

    const Locations getMyAnts() const;

    int32_t getMaxDistance() const;

    void moveAnt(const Location& loc, int direction);

    int32_t getClosestFoodDistance(const Location& loc) const;
    uint32_t countAntsNextTo(const Location& loc) const;
    uint32_t countEnemyAntsInSight(const Location& loc, double range, double* closest) const;

    uint32_t manhattanDistance(const Location &loc1,
            const Location &loc2) const;
    double distance(const Location &loc1, const Location &loc2) const;
    int getDirection(const Location &startLoc, const Location &endLoc) const;
    Location getLocation(const Location &startLoc, int direction) const;

    Locations getLegalMoves(const Location& loc) const;

private:
    /* Map size in rows */
    int32_t mRows;

    /* Map size in columns */
    int32_t mCols;

    /* Maximum possible distance between two points in the map */
    int32_t mMaxDistance;

    Grid mGrid;

    Locations mMyAnts;

    Locations mEnemyAnts;

    Locations mMyHills;

    Locations mEnemyHills;

    Locations mFood;

    Locations mWater;
};

std::ostream& operator<<(std::ostream &os, const Map& map);

#endif /* MAP_H_ */

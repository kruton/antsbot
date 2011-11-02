#ifndef LOCATION_H_
#define LOCATION_H_

#include <vector>

/*
    struct for representing locations in the grid.
*/
struct Location
{
    int row, col;

    Location()
    {
        row = col = 0;
    };

    Location(int r, int c)
    {
        row = r;
        col = c;
    };

    bool operator==(const Location& other) const {
        return row == other.row && col == other.col;
    }
};

typedef std::vector<Location> Locations;

bool operator<(const Location& loc1, const Location& loc2);

std::ostream& operator<<(std::ostream &os, const Location &loc);

#endif //LOCATION_H_

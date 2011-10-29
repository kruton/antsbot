#ifndef LOCATION_H_
#define LOCATION_H_

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

    Location(void* state) {
        size_t xy = reinterpret_cast<size_t>(state);
        row = (int) (xy >> 16) & 0xFFFF;
        col = (int) (xy & 0xFFFF);
    }

    bool operator==(const Location& other) const {
        return row == other.row && col == other.col;
    }

    void* toState() const {
        return (void*) ((row << 16) | col);
    }
};

std::ostream& operator<<(std::ostream &os, const Location &loc);

#endif //LOCATION_H_

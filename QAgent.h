/*
 * QAgent.h
 *
 *  Created on: Oct 31, 2011
 *      Author: kenny
 */

#include <algorithm>
#include <map>

#include "State.h"
#include "Location.h"

#ifndef QAGENT_H_
#define QAGENT_H_

using namespace std;

class QAgent {
    friend ostream& operator<<(ostream& os, const QAgent& agent);
    friend istream& operator>>(istream& is, QAgent& agent);

public:
    QAgent();
    virtual ~QAgent();

    void update(const State& game, const Map& oldMap, const Map& map,
            const Location& oldState, const Location& state, float reward);

    float getQValue(const State& game, const Map& map,
            const Location& state) const;

    Location getAction(const State& game, const Map& map, const Location& loc);

    void writeWeights(const char* filename);

    void readWeights(const char* filename);

private:
    float mEpsilon;
    float mGamma;
    float mAlpha;

    Features mWeights;

    std::map<Location, bool> mSeenState;
};

istream& operator>>(istream& is, QAgent agent);
ostream& operator<<(ostream &os, const QAgent& agent);

template<typename I>
I random_element(I begin, I end);

#endif /* QAGENT_H_ */

/*
 * QAgent.cpp
 *
 *  Created on: Oct 31, 2011
 *      Author: kenny
 */

#include <iostream>
#include <iomanip>
#include <fstream>

#include "QAgent.h"

using namespace std;

QAgent::QAgent()
        : mEpsilon(0.1f)
        , mGamma(0.8f)
        , mAlpha(0.2f)
        , mSeenState()
{
}

QAgent::~QAgent() {
}

float QAgent::getQValue(const State& game, const Map& map, const Location& state) const {
    const Features features = map.extractFeatures(game, state);

    float qValue = 0.0f;

    Features::const_iterator it, fit;
    for (it = features.begin(); it != features.end(); it++) {
        fit = mWeights.find((*it).first);
        if (fit != mWeights.end()) {
            qValue += (*fit).second * (*it).second;
        }
    }

    return qValue;
}

void QAgent::update(const State& game, const Map& oldMap, const Map& map,
        const Location& oldState, const Location& state, float reward) {
    float maxNextQ = 0.0f;

    const Locations possible = map.getLegalMoves(state);
    Locations::const_iterator it;
    for (it = possible.begin(); it < possible.end(); it++) {
        maxNextQ = max(maxNextQ, getQValue(game, map, *it));
    }

    const float qValue = getQValue(game, oldMap, state);

    const Features features = oldMap.extractFeatures(game, oldState);
    Features::const_iterator fit;
    for (fit = features.begin(); fit != features.end(); fit++) {
        if (reward != 0.0f) {
            cerr << "Attempting to update weight " << (*fit).first << " with " << reward << endl;
        }
        mWeights[(*fit).first] += mAlpha * (reward + mGamma * maxNextQ - qValue)
                * (*fit).second;
    }
}

void QAgent::writeWeights(const char* filename) {
    ofstream state;

    state.open(filename);
    Features::const_iterator fit;
    for (fit = mWeights.begin(); fit != mWeights.end(); fit++) {
        state << setprecision(12) << (*fit).first << " " << (*fit).second << endl;
    }

    state << "0 0" << endl;

    state.close();
}

void QAgent::readWeights(const char* filename) {
    ifstream state;

    state.open(filename);
    while (state.good()) {
        int name;
        float value;
        state >> name >> value;
        if (name == 0) {
            break;
        }

        mWeights[(Feature) name] = value;
    }
    state.close();
}

Location QAgent::getAction(const State& game, const Map& map, const Location& loc) {
    const Locations possible = map.getLegalMoves(loc);

    Locations::const_iterator it;

    float chance = (float)rand() / (float) RAND_MAX;
    if (chance <= mEpsilon) {
        return *random_element(possible.begin(), possible.end());
    }

    Locations bestActions;
    float bestQValue = -9999999.9f;
    for (it = possible.begin(); it != possible.end(); it++) {
        float qValue = getQValue(game, map, *it);
        if (qValue > bestQValue) {
            bestActions.clear();
            bestActions.push_back(*it);
            bestQValue = qValue;
        } else if (qValue == bestQValue) {
            bestActions.push_back(*it);
        }
    }

    if (bestActions.size() == 1) {
        return bestActions.at(0);
    } else {
        Locations unseenActions;
        for (it = bestActions.begin(); it != bestActions.end(); it++) {
            if (!mSeenState[*it]) {
               unseenActions.push_back(*it);
            }

        }

        if (unseenActions.size() > 0) {
            it = random_element(unseenActions.begin(), unseenActions.end());
            mSeenState[*it] = true;
        } else {
            it = random_element(bestActions.begin(), bestActions.end());
        }
        return *it;
    }
}

ostream& operator<<(ostream& os, const QAgent& agent) {
    Features::const_iterator fit;
    for (fit = agent.mWeights.begin(); fit != agent.mWeights.end(); fit++) {
        os << setprecision(12) << (*fit).first << " " << (*fit).second << endl;
    }

    os << "0 0" << endl;

    return os;
}

template<typename I>
I random_element(I begin, I end) {
    const unsigned long n = std::distance(begin, end);
//    const unsigned long divisor = (RAND_MAX + 1) / n;

//    unsigned long k;
//    do {
//        k = std::rand() / divisor;
//    } while (k >= n);

//    std::advance(begin, k);

    std::advance(begin, rand() % n);
    return begin;
}

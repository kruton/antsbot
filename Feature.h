/*
 * Features.h
 *
 *  Created on: Nov 1, 2011
 *      Author: kenny
 */

#ifndef FEATURES_H_
#define FEATURES_H_

enum Feature {
	/* Just for ships and gibbles */
	Bias = 1,

	/* Distance to closest food */
	DistanceToFood = 2,

	/* Whether the next step will eat food */
	EatsFood = 3,

	/* Whether the next step will raze an enemy ant hill */
	RazesEnemyHill = 4,

	/* Whether the number of ants within 6 steps is greater than the number of ants next to us */
	Outnumbered = 5,

	/* Whether an enemy is within 6 steps of us */
	EnemyNear = 6,

	/* Whether we have buddies next to us */
	HaveBackup = 7,

	/* Whether we're going to step onto a buddy */
	FriendlyCollision = 8,
};

typedef std::map<Feature, float> Features;

#endif /* FEATURES_H_ */

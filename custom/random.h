#ifndef CUSTOM_RANDOM_H
#define CUSTOM_RANDOM_H

#include <stdint.h>
#include <stdbool.h>

/*Note:
If an implementation for Rng::gen is not inclusive of 1.0 (Issues are reverse if not 0.0 inclusive)
-genBool will be biased slightly towards true (~50.55% chance)
-genRange will not include max.
*/

#define RANDOM_DEFAULT_INCLUSIVE true

typedef struct Rng Rng;
struct Rng {
	double (*gen)(Rng *);
};

typedef struct RngDefault {
	Rng rng;
	uint64_t state;
} RngDefault;

void Rng_init(Rng * rng, double (*gen)(Rng *));
double Rng_gen(Rng * rng);
bool Rng_genBool(Rng * rng);
double Rng_genRange(Rng * rng, double min, double max);
void RngDefault_init(RngDefault * rng, uint64_t seed);

#endif
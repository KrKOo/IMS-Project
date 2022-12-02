#ifndef __TRUCKLIFECYCLE__H__
#define __TRUCKLIFECYCLE__H__

#include <simlib.h>
#include "Configuration.hpp"

class DieselTruckLifecycle : public Process
{
	Store fuelStore;
	Facility truck;
	Stat fuelFilled;

	Configuration config;

	void Behavior();

	void load(int packageCount);
	void unload(int packageCount);
	void travel(double distance);

	void fillFuel();
	void consumeFuel(int fuel);
	double maxTravelDistance();
	double time(double distance);
	int fuel(double distance);

public:
	DieselTruckLifecycle(int id, Configuration config);
};

#endif //!__TRUCKLIFECYCLE__H__
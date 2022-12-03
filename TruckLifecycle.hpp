#ifndef __TRUCKLIFECYCLE__H__
#define __TRUCKLIFECYCLE__H__

#include <simlib.h>
#include "Configuration.hpp"

struct TruckParams
{
	int fuelCapacity;
	int fuelConsumption;
};

class TruckLifecycle : public Process
{
protected:
	Facility truck;

	TruckParams params;
	Configuration config;

	virtual void Behavior() = 0;
	void load(int packageCount);
	void unload(int packageCount);
	void travel(double distance);
	int fillFuel();
	void consumeFuel(int fuel);
	double maxTravelDistance();
	double time(double distance);
	int fuel(double distance);

public:
	TruckLifecycle(int id, TruckParams params, Configuration config);

	Store fuelStore;
	Stat fuelFilled;
	Stat traveledDistance;
	Stat truckParkingTime;
};

#endif //!__TRUCKLIFECYCLE__H__
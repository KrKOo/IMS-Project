#include "DieselTruckLifecycle.hpp"
#include "globals.hpp"

DieselTruckLifecycle::DieselTruckLifecycle(int id, TruckParams params, Configuration config) : TruckLifecycle(id, params, config)
{
}

void DieselTruckLifecycle::Behavior()
{
	fillFuel();

	while (true)
	{
		Way way = ways[Uniform(0, ways.size() - 1)];
		int packageCount = Uniform(config.truckCargoCapacityMin, config.truckCargoCapacityMax);

		Seize(truck);

		int startTime = Time;
		Seize(loadingDock);
		int endTime = Time;
		truckParkingTime(endTime - startTime);

		load(packageCount);
		Release(loadingDock);

		travel(way);
		traveledDistance(way.distance);

		unload(packageCount);
		packagesDelivered += packageCount;

		travel(way);
		traveledDistance(way.distance);

		Release(truck);
	}
}

void DieselTruckLifecycle::travel(Way way)
{
	double speed = way.distance / way.time;
	if (way.distance > maxTravelDistance())
	{
		// Not enough fuel for the whole drive
		double distanceToFuelingStation = Uniform(0, maxTravelDistance());
		consumeFuel(fuel(distanceToFuelingStation));

		Wait(distanceToFuelingStation / speed);

		// At the fueling station
		int filledAmount = fillFuel();
		fuelFilled(filledAmount);
		Wait(20);

		// Continue to the destination
		double remainingDistance = way.distance - distanceToFuelingStation;
		consumeFuel(fuel(remainingDistance));
		Wait(remainingDistance / speed);
	}
	else
	{
		// Enough fuel for the whole drive
		consumeFuel(fuel(way.distance));
		Wait(way.time);
	}
}

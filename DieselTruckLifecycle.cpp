#include "DieselTruckLifecycle.hpp"
#include "globals.hpp"

DieselTruckLifecycle::DieselTruckLifecycle(int id, Configuration config) : TruckLifecycle(id, config)
{
}

void DieselTruckLifecycle::Behavior()
{
	fillFuel();

	while (true)
	{
		double distance = Uniform(config.travelDistanceMin, config.travelDistanceMax);
		int packageCount = Uniform(config.truckCargoCapacityMin, config.truckCargoCapacityMax);

		int startTime = Time;
		Seize(truck);
		int endTime = Time;

		truckParkingTime(endTime - startTime);

		truckPackageCountHistogram(packageCount);
		load(packageCount);

		travel(distance);
		traveledDistance(distance);

		unload(packageCount);
		packagesDelivered += packageCount;

		travel(distance);
		traveledDistance(distance);

		Release(truck);
	}
}

void DieselTruckLifecycle::travel(double distance)
{
	if (distance > maxTravelDistance())
	{
		// Not enough fuel for the whole drive
		double distanceToFuelingStation = Uniform(0, maxTravelDistance());
		consumeFuel(fuel(distanceToFuelingStation));
		Wait(time(distanceToFuelingStation));

		// At the fueling station
		int filledAmount = fillFuel();
		fuelFilled(filledAmount);
		Wait(60);

		// Continue to the destination
		double remainingDistance = distance - distanceToFuelingStation;
		consumeFuel(fuel(remainingDistance));
		Wait(time(remainingDistance));
	}
	else
	{
		// Enough fuel for the whole drive
		consumeFuel(fuel(distance));
		Wait(time(distance));
	}
}

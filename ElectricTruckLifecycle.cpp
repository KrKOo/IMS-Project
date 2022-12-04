#include "ElectricTruckLifecycle.hpp"
#include "globals.hpp"

ElectricTruckLifecycle::ElectricTruckLifecycle(int id, TruckParams params, Configuration config) : TruckLifecycle(id, params, config)
{
	electricityChargedAtFactory.SetName("Electricity charged at factory");
	electricityChargedAtDestination.SetName("Electricity charged at destination");
}

void ElectricTruckLifecycle::Behavior()
{
	TruckLifecycle::fillFuel();

	while (true)
	{
		Way way = ways[Uniform(0, ways.size() - 1)];
		int packageCount = Uniform(config.truckCargoCapacityMin, config.truckCargoCapacityMax);

		Seize(truck);

		// 2 * distance because we need to go there and back
		int filledAtFactory = checkFuelAndFuelUpIfNeeded(2 * way.distance);
		if (filledAtFactory > 0)
		{
			electricityChargedAtFactory(filledAtFactory);
		}

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

		// Charge up, if we previously couldn't charge it more
		int filledAtDestination = checkFuelAndFuelUpIfNeeded(way.distance);
		if (filledAtDestination > 0)
		{
			electricityChargedAtDestination(filledAtDestination);
		}

		travel(way);
		traveledDistance(way.distance);

		Release(truck);
	}
}

int ElectricTruckLifecycle::checkFuelAndFuelUpIfNeeded(double distance)
{
	int filled = 0;
	if (distance > maxTravelDistance())
	{
		int initialFuel = fuelStore.Used();
		int newFuelLevel = fillFuel(distance);
		filled = newFuelLevel - initialFuel;
		Wait(fuelingTime(initialFuel, newFuelLevel));
	}

	return filled;
}

int ElectricTruckLifecycle::fillFuel(double distance)
{
	int maxFuelToFill = params.fuelCapacity - fuelStore.Used();

	int fuelToFill = fuel(distance) > maxFuelToFill ? maxFuelToFill : fuel(distance);

	Enter(fuelStore, fuelToFill);

	return fuelStore.Used();
}

// function sqrt(x) <0-1> * MAX_CHARGE_TIME
double ElectricTruckLifecycle::fuelingTime(double initialLevel, double finalLevel)
{
	double initialLevelPercentage = initialLevel / params.fuelCapacity;
	double finalLevelPercentage = finalLevel / params.fuelCapacity;

	double initialTimePercentage = initialLevelPercentage * initialLevelPercentage;
	double finalTimePercentage = finalLevelPercentage * finalLevelPercentage;

	return (finalTimePercentage - initialTimePercentage) * MAX_CHARGE_TIME;
}
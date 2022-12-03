#include "ElectricTruckLifecycle.hpp"
#include "globals.hpp"

ElectricTruckLifecycle::ElectricTruckLifecycle(int id, Configuration config) : TruckLifecycle(id, config)
{
	electricityChargedAtFactory.SetName("Electricity charged at factory");
	electricityChargedAtDestination.SetName("Electricity charged at destination");
}

void ElectricTruckLifecycle::Behavior()
{
	TruckLifecycle::fillFuel();

	while (true)
	{
		double distance = Uniform(config.travelDistanceMin, config.travelDistanceMax);
		int packageCount = Uniform(config.truckCargoCapacityMin, config.truckCargoCapacityMax);

		int startTime = Time;
		Seize(truck);
		int endTime = Time;

		truckParkingTime(endTime - startTime);
		truckPackageCountHistogram(packageCount);

		// 2 * distance because we need to go there and back
		int filledAtFactory = checkFuelAndFuelUpIfNeeded(2 * distance);
		if (filledAtFactory > 0)
		{
			electricityChargedAtFactory(filledAtFactory);
		}

		load(packageCount);
		travel(distance);
		traveledDistance(distance);
		unload(packageCount);
		packagesDelivered += packageCount;

		// Charge up, if we previously couldn't charge it more
		int filledAtDestination = checkFuelAndFuelUpIfNeeded(distance);
		if (filledAtDestination > 0)
		{
			electricityChargedAtDestination(filledAtDestination);
		}

		travel(distance);
		traveledDistance(distance);

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
		Wait(60);
	}

	return filled;
}

int ElectricTruckLifecycle::fillFuel(double distance)
{
	int maxFuelToFill = config.truckFuelTankCapacity - fuelStore.Used();

	int fuelToFill = fuel(distance) > maxFuelToFill ? maxFuelToFill : fuel(distance);

	Enter(fuelStore, fuelToFill);

	return fuelStore.Used();
}
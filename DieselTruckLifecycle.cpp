#include "DieselTruckLifecycle.hpp"

extern Store warehouse;
extern Facility loadingDock;
extern Queue truckParkingQueue;

// Stats
extern Stat truckParkingTime;
extern Histogram truckPackageCountHistogram;
extern int packagesDelivered;

DieselTruckLifecycle::DieselTruckLifecycle(int id, Configuration config)
{
	this->config = config;
	truck.SetName("Truck " + std::to_string(id));
	truck.SetQueue(truckParkingQueue);
	fuelStore.SetName("FuelStore " + std::to_string(id));
	fuelStore.SetCapacity(config.truckFuelTankCapacity);
}

void DieselTruckLifecycle::Behavior()
{
	fillFuel();

	while (true)
	{
		double distance = Uniform(10, 300);
		int packageCount = Uniform(config.truckCargoCapacityMin, config.truckCargoCapacityMax);

		int startTime = Time;
		Seize(truck);
		int endTime = Time;

		truckParkingTime(endTime - startTime);

		truckPackageCountHistogram(packageCount);
		load(packageCount);

		travel(distance);

		unload(packageCount);
		packagesDelivered += packageCount;

		travel(distance);

		Release(truck);
	}
}

void DieselTruckLifecycle::load(int packageCount)
{
	Seize(loadingDock);
	Enter(warehouse, packageCount);
	Wait(packageCount * config.packageLoadTime);
	Release(loadingDock);
}

void DieselTruckLifecycle::unload(int packageCount)
{
	// Unloading
	Wait(packageCount * config.packageUnloadTime);

	// Waiting for paperwork/in queue on the depo
	Wait(Uniform(config.unloadExtraTimeMin, config.unloadExtraTimeMax));
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
		fillFuel();
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

void DieselTruckLifecycle::fillFuel()
{
	int fuelToFill = config.truckFuelTankCapacity - fuelStore.Used();
	Enter(fuelStore, fuelToFill);
	fuelFilled(fuelToFill);
}

void DieselTruckLifecycle::consumeFuel(int fuel)
{
	Leave(fuelStore, fuel);
}

double DieselTruckLifecycle::maxTravelDistance()
{
	return fuelStore.Used() / config.truckFuelConsumption;
}

double DieselTruckLifecycle::time(double distance) // km -> min
{
	return distance / (60 / 70.0); // 70km/h
}

int DieselTruckLifecycle::fuel(double distance)
{
	return distance * config.truckFuelConsumption;
}
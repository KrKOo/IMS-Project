#include "TruckLifecycle.hpp"

extern Store warehouse;
extern Facility loadingDock;
extern Queue truckParkingQueue;

// Stats
extern Stat truckParkingTime;
extern Histogram truckPackageCountHistogram;
extern int packagesDelivered;

TruckLifecycle::TruckLifecycle(int id, Configuration config)
{
	this->config = config;
	truck.SetName("Truck " + std::to_string(id));
	truck.SetQueue(truckParkingQueue);
	fuelStore.SetName("FuelStore " + std::to_string(id));
	fuelStore.SetCapacity(config.truckFuelTankCapacity);
}

void TruckLifecycle::load(int packageCount)
{
	Seize(loadingDock);
	Enter(warehouse, packageCount);
	Wait(packageCount * config.packageLoadTime);
	Release(loadingDock);
}

void TruckLifecycle::unload(int packageCount)
{
	// Unloading
	Wait(packageCount * config.packageUnloadTime);

	// Waiting for paperwork/in queue on the depo
	Wait(Uniform(config.unloadExtraTimeMin, config.unloadExtraTimeMax));
}

void TruckLifecycle::travel(double distance)
{
	Wait(time(distance));
	consumeFuel(fuel(distance));
}

int TruckLifecycle::fillFuel()
{
	int fuelToFill = config.truckFuelTankCapacity - fuelStore.Used();
	Enter(fuelStore, fuelToFill);
	return fuelToFill;
}

void TruckLifecycle::consumeFuel(int fuel)
{
	Leave(fuelStore, fuel);
}

double TruckLifecycle::maxTravelDistance()
{
	return fuelStore.Used() / config.truckFuelConsumption;
}

double TruckLifecycle::time(double distance) // km -> min
{
	return distance / (60 / 70.0); // 70km/h
}

int TruckLifecycle::fuel(double distance)
{
	return distance * config.truckFuelConsumption;
}

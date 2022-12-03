#include "TruckLifecycle.hpp"

extern Store warehouse;
extern Facility loadingDock;
extern Queue truckParkingQueue;

// Stats
extern Histogram truckPackageCountHistogram;
extern int packagesDelivered;

TruckLifecycle::TruckLifecycle(int id, TruckParams params, Configuration config)
{
	this->params = params;
	this->config = config;
	truck.SetName("Truck " + std::to_string(id));
	truck.SetQueue(truckParkingQueue);
	fuelStore.SetName("FuelStore " + std::to_string(id));
	fuelStore.SetCapacity(params.fuelCapacity);
}

void TruckLifecycle::load(int packageCount)
{
	Enter(warehouse, packageCount);
	Wait(packageCount * config.packageLoadTime);
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
	int fuelToFill = params.fuelCapacity - fuelStore.Used();
	Enter(fuelStore, fuelToFill);
	return fuelToFill;
}

void TruckLifecycle::consumeFuel(int fuel)
{
	Leave(fuelStore, fuel);
}

double TruckLifecycle::maxTravelDistance()
{
	return fuelStore.Used() / (double)params.fuelConsumption;
}

double TruckLifecycle::time(double distance) // km -> min
{
	return distance / (60 / 70.0); // 70km/h
}

int TruckLifecycle::fuel(double distance)
{
	return distance * params.fuelConsumption;
}

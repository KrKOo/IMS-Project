#ifndef __CONFIGURATION__H__
#define __CONFIGURATION__H__

struct Configuration
{
	// Time
	int simulationDuration;
	// Truck
	int truckCount;
	int truckFuelTankCapacity;
	double truckFuelConsumption;
	int truckCargoCapacityMin;
	int truckCargoCapacityMax;
	// Factory
	int warehouseCapacity;
	int packageManufacturingTime;
	// Loading
	int packageLoadTime;
	// Unloading
	int packageUnloadTime;
	int unloadExtraTimeMin;
	int unloadExtraTimeMax;
};

#endif //!__CONFIGURATION__H__
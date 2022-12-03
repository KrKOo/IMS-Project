#ifndef __CONFIGURATION__H__
#define __CONFIGURATION__H__

struct Configuration
{
	// Time
	int simulationDuration;
	// Truck
	// int dieselFuelCapacity;
	// int electricFuelCapacity;
	// int dieselFuelConsumption;
	// int electricFuelConsumption;
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
	// Travel
	int travelDistanceMin;
	int travelDistanceMax;
};

#endif //!__CONFIGURATION__H__
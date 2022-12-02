#include <iostream>
#include <simlib.h>

#include "Configuration.hpp"
#include "DieselTruckLifecycle.hpp"

Configuration config = {
	.simulationDuration = 1 * 365 * 24 * 60,
	.truckCount = 10,
	.truckFuelTankCapacity = 1000,
	.truckFuelConsumption = 0.4,
	.truckCargoCapacityMin = 7,
	.truckCargoCapacityMax = 20,
	.warehouseCapacity = 100000,
	.packageManufacturingTime = 3,
	.packageLoadTime = 3,
	.packageUnloadTime = 3,
	.unloadExtraTimeMin = 5,
	.unloadExtraTimeMax = 20,
};

Store warehouse("Warehouse", config.warehouseCapacity);

Queue loadingDockQueue("Loading Dock Queue");
Facility loadingDock(loadingDockQueue);
Queue truckParkingQueue("Truck Parking Queue");

Histogram truckPackageCountHistogram(0.0, 1.0, 21);
Stat truckParkingTime("Truck Parking Time");

Stat testStat("Test stat");

int packagesManufactured = 0;
int packagesDelivered = 0;

double fuelConsumption = 0.4;

class PackageGenerator : public Process
{
	void Behavior()
	{
		if (warehouse.Used() > 0)
		{
			Leave(warehouse, 1);
			packagesManufactured += 1;
		}
	}
};

class InitProcess : public Process
{
	void Behavior()
	{
		Enter(warehouse, config.warehouseCapacity); // Empty warehouse
		for (int i = 0; i < config.truckCount; i++)
		{
			(new DieselTruckLifecycle(i, config))->Activate();
		}
	}
};

class PackageGeneratorEvent : public Event
{
	void Behavior()
	{
		(new PackageGenerator)->Activate();

		Activate(Time + config.packageManufacturingTime);
	}
};

int main()
{
	Init(0, config.simulationDuration);

	(new InitProcess)->Activate(0);
	(new PackageGeneratorEvent)->Activate(1);

	Run();
	loadingDock.Output();
	truckParkingQueue.Output();
	int totalFuelFilled = 0;
	for (int i = 0; i < config.truckCount; i++)
	{
		// totalFuelFilled = truckFuelFilled[i].Sum();
	}
	truckParkingTime.Output();

	truckPackageCountHistogram.Output();
	std::cout << "Packages manufactured: " << packagesManufactured << std::endl;
	std::cout << "Packages delivered: " << packagesDelivered << std::endl;
	std::cout << "Total fuel bought: " << totalFuelFilled << std::endl;

	testStat.Output();

	return 0;
}
#include <iostream>
#include <simlib.h>
#include <vector>

#include "Configuration.hpp"
#include "DieselTruckLifecycle.hpp"
#include "ElectricTruckLifecycle.hpp"
#include "globals.hpp"

Configuration config = {
	.simulationDuration = 1 * 365 * 24 * 60,
	.truckCount = 10,
	.truckFuelTankCapacity = 540000,
	.truckFuelConsumption = 1000,
	.truckCargoCapacityMin = 7,
	.truckCargoCapacityMax = 20,
	.warehouseCapacity = 100000,
	.packageManufacturingTime = 3,
	.packageLoadTime = 3,
	.packageUnloadTime = 3,
	.unloadExtraTimeMin = 5,
	.unloadExtraTimeMax = 20,
	.travelDistanceMin = 10,
	.travelDistanceMax = 400,
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

	std::vector<ElectricTruckLifecycle *> trucks;
	for (auto i = 0; i < config.truckCount; i++)
	{
		auto truckLifecycle = new ElectricTruckLifecycle(i, config);
		truckLifecycle->Activate(1);
		trucks.push_back(truckLifecycle);
	}

	Run();
	loadingDock.Output();
	truckParkingQueue.Output();
	unsigned long totalFuelFilled = 0;
	unsigned long totalTraveledDistance = 0;
	unsigned long totalElectricityChargedAtFactory = 0;
	unsigned long totalElectricityChargedAtDestination = 0;
	unsigned long totalParkingTime = 0;

	for (auto &truck : trucks)
	{
		totalParkingTime += truck->truckParkingTime.Sum();
		totalFuelFilled += truck->fuelFilled.Sum();
		totalTraveledDistance += truck->traveledDistance.Sum();
		totalElectricityChargedAtFactory += truck->electricityChargedAtFactory.Sum();
		totalElectricityChargedAtDestination += truck->electricityChargedAtDestination.Sum();
		std::cout << truck->fuelFilled.Sum() << "   " << truck->traveledDistance.Sum() << std::endl;
	}

	truckParkingTime.Output();

	truckPackageCountHistogram.Output();
	std::cout << "Packages manufactured: " << packagesManufactured << std::endl;
	std::cout << "Packages delivered: " << packagesDelivered << std::endl;
	std::cout << "Total fuel bought: " << totalFuelFilled << std::endl;
	std::cout << "Total electricity charged at factory: " << totalElectricityChargedAtFactory << std::endl;
	std::cout << "Total electricity charged at destination: " << totalElectricityChargedAtDestination << std::endl;
	std::cout << "Total traveled distance: " << totalTraveledDistance << std::endl;
	std::cout << "Total parking time: " << totalParkingTime << std::endl;

	testStat.Output();

	return 0;
}
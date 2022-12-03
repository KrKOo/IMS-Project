#include <iostream>
#include <simlib.h>
#include <vector>

#include "Configuration.hpp"
#include "DieselTruckLifecycle.hpp"
#include "ElectricTruckLifecycle.hpp"
#include "ArgumentParser.hpp"
#include "globals.hpp"

Configuration config = {
	.simulationDuration = 1 * 365 * 24 * 60,
	.truckCount = 10,
	.truckFuelTankCapacity = 540000, // 540kWh
	.truckFuelConsumption = 11,		 // 1.1kWh/100km
	.truckCargoCapacityMin = 7,
	.truckCargoCapacityMax = 20,
	.warehouseCapacity = 100000,
	.packageManufacturingTime = 3,
	.packageLoadTime = 3,
	.packageUnloadTime = 3,
	.unloadExtraTimeMin = 5,
	.unloadExtraTimeMax = 20,
	.travelDistanceMin = 230,
	.travelDistanceMax = 260,
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

void simulate(Arguments args)
{
	config.truckCount = args.truckCount;
	config.truckFuelTankCapacity = args.fuelTankCapacity;
	config.truckFuelConsumption = args.fuelConsumption;
	config.packageManufacturingTime = args.packageManufacturingTime;
	config.simulationDuration = args.simulationDuration;

	Init(0, config.simulationDuration);

	(new InitProcess)->Activate(0);
	(new PackageGeneratorEvent)->Activate(1);

	std::vector<TruckLifecycle *> trucks;
	for (auto i = 0; i < config.truckCount; i++)
	{
		TruckLifecycle *truckLifecycle;
		if (args.isElectric)
		{
			truckLifecycle = new ElectricTruckLifecycle(i, config);
		}
		else
		{
			truckLifecycle = new DieselTruckLifecycle(i, config);
		}

		trucks.push_back(truckLifecycle);
		truckLifecycle->Activate(1);
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
		if (args.isElectric)
		{
			ElectricTruckLifecycle *electricTruck = static_cast<ElectricTruckLifecycle *>(truck);
			totalElectricityChargedAtFactory += electricTruck->electricityChargedAtFactory.Sum();
			totalElectricityChargedAtDestination += electricTruck->electricityChargedAtDestination.Sum();
		}
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
}

int main(int argc, char **argv)
{
	Arguments defaultArguments = {
		.truckCount = 4,
		.fuelTankCapacity = 1000000, // 1000l
		.fuelConsumption = 350,		 // 35l/100km
		.isElectric = false,
		.packageManufacturingTime = 3,
		.simulationDuration = 1 * 365 * 24 * 60,
	};

	ArgumentParser argumentParser(argc, argv);
	Arguments arguments = argumentParser.parse(defaultArguments);

	simulate(arguments);
	return 0;
}
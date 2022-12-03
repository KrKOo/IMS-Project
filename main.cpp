#include <iostream>
#include <simlib.h>
#include <vector>
#include <memory>

#include "Configuration.hpp"
#include "DieselTruckLifecycle.hpp"
#include "ElectricTruckLifecycle.hpp"
#include "ArgumentParser.hpp"
#include "globals.hpp"

Configuration config = {
	.simulationDuration = 1 * 365 * 24 * 60,
	// .dieselFuelCapacity = 1000000,	 // 1000l
	// .electricFuelCapacity = 540000,	 // 540kWh
	// .dieselFuelConsumption = 350,	 // 35l/100km
	// .electricFuelConsumption = 1100, // 1.1kWh/100km
	.truckCargoCapacityMin = 7,
	.truckCargoCapacityMax = 20,
	.warehouseCapacity = 100000000,
	.packageManufacturingTime = 3,
	.packageLoadTime = 3,
	.packageUnloadTime = 3,
	.unloadExtraTimeMin = 5,
	.unloadExtraTimeMax = 20,
	.travelDistanceMin = 10,
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
	config.packageManufacturingTime = args.packageManufacturingTime;
	config.simulationDuration = args.simulationDuration;

	Init(0, config.simulationDuration);

	(new InitProcess)->Activate(0);
	(new PackageGeneratorEvent)->Activate(1);

	std::vector<DieselTruckLifecycle *> dieselTrucks;
	for (auto i = 0; i < args.dieselTruckCount; i++)
	{
		TruckParams params = {
			.fuelCapacity = args.dieselFuelCapacity,
			.fuelConsumption = args.dieselFuelConsumption,
		};
		auto truckLifecycle = new DieselTruckLifecycle(i, params, config);
		dieselTrucks.push_back(truckLifecycle);
		truckLifecycle->Activate(1);
	}

	std::vector<ElectricTruckLifecycle *> electricTrucks;
	for (auto i = 0; i < args.electricTruckCount; i++)
	{
		TruckParams params = {
			.fuelCapacity = args.electricFuelCapacity,
			.fuelConsumption = args.electricFuelConsumption,
		};
		auto truckLifecycle = new ElectricTruckLifecycle(i, params, config);
		electricTrucks.push_back(truckLifecycle);
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

	for (DieselTruckLifecycle *dieselTruck : dieselTrucks)
	{
		totalFuelFilled += dieselTruck->fuelFilled.Sum();
		totalTraveledDistance += dieselTruck->traveledDistance.Sum();
		totalParkingTime += dieselTruck->truckParkingTime.Sum();
	}

	for (ElectricTruckLifecycle *electricTruck : electricTrucks)
	{
		totalElectricityChargedAtFactory += electricTruck->electricityChargedAtFactory.Sum();
		totalElectricityChargedAtDestination += electricTruck->electricityChargedAtDestination.Sum();
		totalTraveledDistance += electricTruck->traveledDistance.Sum();
		totalParkingTime += electricTruck->truckParkingTime.Sum();
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
		.dieselTruckCount = 0,
		.electricTruckCount = 0,
		.dieselFuelCapacity = 1000000,	 // 1000l
		.electricFuelCapacity = 540000,	 // 540kWh
		.dieselFuelConsumption = 350,	 // 35l/100km
		.electricFuelConsumption = 1100, // 1.1kWh/100km
		.packageManufacturingTime = 3,
		.simulationDuration = 1 * 365 * 24 * 60,
	};

	ArgumentParser argumentParser(argc, argv);
	Arguments arguments = argumentParser.parse(defaultArguments);

	simulate(arguments);
	return 0;
}
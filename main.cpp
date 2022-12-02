#include <iostream>
#include <simlib.h>

// Time
const int SIMULATION_TIME = 1 * 365 * 24 * 60; // 1 year

// Truck
const int TRUCK_COUNT = 10;
const int FUEL_CAPACITY = 1000;
const int TRUCK_CAPACITY_MAX = 20;
const int TRUCK_CAPACITY_MIN = 7;

// Factory
const int WAREHOUSE_CAPACITY = 100000;
const int PACKAGE_MANUFACTURING_TIME = 10; // min

// Loading
const int PACKAGE_LOAD_TIME = 3; // min

// Unloading
const int PACKAGE_UNLOAD_TIME = 3;	  // min
const int UNLOAD_EXTRA_TIME_MIN = 5;  // min
const int UNLOAD_EXTRA_TIME_MAX = 20; // min

Store fuelStore[TRUCK_COUNT];
Store warehouse("Warehouse", WAREHOUSE_CAPACITY);

Queue loadingDockQueue("Loading Dock Queue");
Facility loadingDock(loadingDockQueue);
Facility trucks[TRUCK_COUNT];
Queue truckParkingQueue("Truck Parking Queue");

Histogram truckPackageCountHistogram(0.0, 1.0, 21);
Stat truckParkingTime("Truck Parking Time");
Stat truckFuelFilled[TRUCK_COUNT];

Stat testStat("Test stat");
int packagesManufactured = 0;
int packagesDelivered = 0;

double fuelConsumption = 0.4;

class TruckLifecycle : public Process
{
public:
	int truckId;
	TruckLifecycle(int truckId)
	{
		this->truckId = truckId;
	}

	void Behavior()
	{

		fillFuel();

		while (true)
		{
			double distance = Uniform(10, 300);
			int packageCount = Uniform(TRUCK_CAPACITY_MIN, TRUCK_CAPACITY_MAX);

			int startTime = Time;
			Seize(trucks[truckId]);
			int endTime = Time;

			truckParkingTime(endTime - startTime);

			truckPackageCountHistogram(packageCount);
			load(packageCount);

			travel(distance);

			unload(packageCount);
			packagesDelivered += packageCount;

			travel(distance);

			Release(trucks[truckId]);
		}
	}

	void load(int packageCount)
	{
		Seize(loadingDock);
		Enter(warehouse, packageCount);
		Wait(packageCount * PACKAGE_LOAD_TIME);
		Release(loadingDock);
	}

	void unload(int packageCount)
	{
		// Unloading
		Wait(packageCount * PACKAGE_UNLOAD_TIME);

		// Waiting for paperwork/in queue on the depo
		Wait(Uniform(5, 20));
	}

	void travel(double distance)
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

	void fillFuel()
	{
		int fuelToFill = FUEL_CAPACITY - fuelStore[truckId].Used();
		Enter(fuelStore[truckId], fuelToFill);
		truckFuelFilled[truckId](fuelToFill);
	}

	void consumeFuel(int fuel)
	{
		Leave(fuelStore[truckId], fuel);
	}

	double maxTravelDistance()
	{
		return fuelStore[truckId].Used() / fuelConsumption;
	}

	double time(double distance) // km -> min
	{
		return distance / (60 / 70.0); // 70km/h
	}

	int fuel(double distance)
	{
		return distance * fuelConsumption;
	}
};

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
		Enter(warehouse, WAREHOUSE_CAPACITY); // Empty warehouse
		for (int i = 0; i < TRUCK_COUNT; i++)
		{
			(new TruckLifecycle(i))->Activate();
		}
	}
};

class PackageGeneratorEvent : public Event
{
	void Behavior()
	{
		(new PackageGenerator)->Activate();

		Activate(Time + PACKAGE_MANUFACTURING_TIME);
	}
};

int main()
{
	for (int i = 0; i < TRUCK_COUNT; i++)
	{
		trucks[i].SetName("Truck" + std::to_string(i));
		trucks[i].SetQueue(truckParkingQueue);
		truckFuelFilled[i].SetName("Truck Fuel Filled " + std::to_string(i));
		fuelStore[i].SetName("Fuel Store " + std::to_string(i));
		fuelStore[i].SetCapacity(FUEL_CAPACITY);
	}

	Init(0, SIMULATION_TIME);

	(new InitProcess)->Activate(0);
	(new PackageGeneratorEvent)->Activate(1);

	Run();
	loadingDock.Output();
	truckParkingQueue.Output();
	int totalFuelFilled = 0;
	for (int i = 0; i < TRUCK_COUNT; i++)
	{
		trucks[i].Output();
		fuelStore[i].Output();
		truckFuelFilled[i].Output();
		totalFuelFilled = truckFuelFilled[i].Sum();
	}
	truckParkingTime.Output();

	truckPackageCountHistogram.Output();
	std::cout << "Packages manufactured: " << packagesManufactured << std::endl;
	std::cout << "Packages delivered: " << packagesDelivered << std::endl;
	std::cout << "Total fuel bought: " << totalFuelFilled << std::endl;

	testStat.Output();

	return 0;
}
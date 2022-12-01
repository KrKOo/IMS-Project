#include <iostream>
#include <simlib.h>

const int FUEL_CAPACITY = 1000;
const int TRUCK_CAPACITY = 20;
const int TRUCK_COUNT = 2;
const int PACKAGE_LOAD_TIME = 3; // min
const int WAREHOUSE_CAPACITY = 10000;

Store fuelStore[TRUCK_COUNT];
Store warehouse("Warehouse", WAREHOUSE_CAPACITY);
Store truckCargo[TRUCK_COUNT];

Queue loadingDockQueue("Loading Dock Queue");
Facility loadingDock(loadingDockQueue);
Facility trucks[TRUCK_COUNT];
Queue truckParkingQueue("Truck Parking Queue");
Stat truckParkingTime("Truck Parking Time");
Stat truckFuelUsage[TRUCK_COUNT];

double fuelConsumption = 0.4;

double maxTravelDistance(int truckId)
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
		fuelStore[truckId].SetName("Fuel Store " + std::to_string(truckId));
		fuelStore[truckId].SetCapacity(FUEL_CAPACITY);
		fillFuel();

		while (true)
		{
			int startTime = Time;
			Seize(trucks[truckId]);

			int endTime = Time;
			truckParkingTime(endTime - startTime);

			load();

			travel(false, truckId);
		}
	}

	void load()
	{
		int packagesToLoad = Uniform(7, TRUCK_CAPACITY);

		Seize(loadingDock);
		Enter(warehouse, packagesToLoad);
		Enter(truckCargo[truckId], packagesToLoad);
		Wait(packagesToLoad * PACKAGE_LOAD_TIME);
		Release(loadingDock);
	}

	void travel(bool isTravelingHome, int truckId)
	{
		double distance = Uniform(10, 300);
		if (distance > maxTravelDistance(truckId))
		{
			// Not enough fuel for the whole drive
			double distanceToFuelingStation = Uniform(0, maxTravelDistance(truckId));
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

		if (isTravelingHome)
		{
			Release(trucks[truckId]);
		}
		else
		{
			Wait(60);
			travel(true, truckId);
		}
	}

	void fillFuel()
	{
		int fuelToFill = FUEL_CAPACITY - fuelStore[truckId].Used();
		Enter(fuelStore[truckId], fuelToFill);
		truckFuelUsage[truckId](fuelToFill);
	}

	void consumeFuel(int fuel)
	{
		Leave(fuelStore[truckId], fuel);
	}
};

class PackageGenerator : public Process
{
public:
	PackageGenerator()
	{
		Enter(warehouse, WAREHOUSE_CAPACITY);
	}
	void Behavior()
	{
		Leave(warehouse, 1);
		Activate(Time + Exponential(10));
	}
};

class Delivery : public Event
{
	void Behavior()
	{
		(new PackageGenerator)->Activate();
		for (int i = 0; i < TRUCK_COUNT; i++)
		{
			(new TruckLifecycle(i))->Activate();
		}
	}
};

int main()
{
	for (int i = 0; i < TRUCK_COUNT; i++)
	{
		trucks[i].SetName("Truck" + std::to_string(i));
		trucks[i].SetQueue(truckParkingQueue);
		truckFuelUsage[i].SetName("Truck Fuel Usage " + std::to_string(i));
		truckCargo[i].SetName("Truck Cargo " + std::to_string(i));
	}

	Init(0, 10000);
	(new PackageGenerator)->Activate();
	(new Delivery)->Activate();

	Run();
	loadingDock.Output();
	truckParkingQueue.Output();
	int totalFuelUsage = 0;
	for (int i = 0; i < TRUCK_COUNT; i++)
	{
		trucks[i].Output();
		fuelStore[i].Output();
		truckFuelUsage[i].Output();
		truckCargo[i].Output();
		totalFuelUsage = truckFuelUsage[i].Sum();
	}
	truckParkingTime.Output();
	std::cout << "Total fuel usage: " << totalFuelUsage << std::endl;

	return 0;
}
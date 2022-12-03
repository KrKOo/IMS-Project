#ifndef __ARGUMENTPARSER__H__
#define __ARGUMENTPARSER__H__

#define INVALID_ARUMENT_ERROR 2
#define MISSING_ARGUMENT_ERROR 3

struct Arguments
{
	int dieselTruckCount;
	int electricTruckCount;

	int dieselFuelCapacity;
	int electricFuelCapacity;

	int dieselFuelConsumption;
	int electricFuelConsumption;

	int packageManufacturingTime;
	int simulationDuration;
};

class ArgumentParser
{
private:
	// Argument count
	int argc;

	// Argument values
	char **argv;

	// Helper function to check for arguments in an optional argument
	bool optionalArgumentIsPresent();

	// Get the value of the argument
	std::string getArgument();

	// Get the value of the argument as a number
	int getNumericArgument();

	// Print help
	void printHelp();

public:
	ArgumentParser(int argc, char **argv);

	// Parse the arguments
	Arguments parse(Arguments &arguments);
};

#endif //!__ARGUMENTPARSER__H__
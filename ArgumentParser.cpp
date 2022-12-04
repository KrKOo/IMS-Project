#include <iostream>
#include <getopt.h>

#include "ArgumentParser.hpp"

#define DIESEL_CAPACITY 1000
#define ELECTRIC_CAPACITY 1002
#define DIESEL_FUEL_CONSUMPTION 1003
#define ELECTRIC_FUEL_CONSUMPTION 1004

ArgumentParser::ArgumentParser(int argc, char **argv)
{
	this->argc = argc;
	this->argv = argv;
}

// Modified example from
// https://www.man7.org/linux/man-pages/man3/getopt.3.html
Arguments ArgumentParser::parse(Arguments &defaultArguments)
{
	Arguments arguments = defaultArguments;
	const char *shortOptions = "hd:e:s:m";
	struct option longOptions[] = {
		{"help", no_argument, 0, 'h'},
		{"df", required_argument, 0, DIESEL_CAPACITY},
		{"ef", required_argument, 0, ELECTRIC_CAPACITY},
		{"dc", required_argument, 0, DIESEL_FUEL_CONSUMPTION},
		{"ec", required_argument, 0, ELECTRIC_FUEL_CONSUMPTION},
		{0, 0, 0, 0}};

	int opt;
	int optIndex;
	while ((opt = getopt_long(this->argc, this->argv, shortOptions, longOptions, &optIndex)) != -1)
	{
		switch (opt)
		{
		case 0:
			break;
		case 'h':
			printHelp();
			exit(0);
		case 'd':
			arguments.dieselTruckCount = getNumericArgument();
			break;
		case 'e':
			arguments.electricTruckCount = getNumericArgument();
			break;
		case DIESEL_CAPACITY:
			arguments.dieselFuelCapacity = getNumericArgument();
			break;
		case ELECTRIC_CAPACITY:
			arguments.electricFuelCapacity = getNumericArgument();
			break;
		case DIESEL_FUEL_CONSUMPTION:
			arguments.dieselFuelConsumption = getNumericArgument();
			break;
		case ELECTRIC_FUEL_CONSUMPTION:
			arguments.electricFuelConsumption = getNumericArgument();
			break;
		case 's':
			arguments.simulationDuration = getNumericArgument();
			break;
		case 'm':
			arguments.packageManufacturingTime = getNumericArgument();
			break;
		case '?':
			/* getopt_long already printed an error message. */
			exit(INVALID_ARUMENT_ERROR);
			break;

		default:
			abort();
		}
	}

	return arguments;
}

void ArgumentParser::printHelp()
{
	std::cout << "-d <count> Diesel truck count" << std::endl;
	std::cout << "-e <count> Electric truck count" << std::endl;
	std::cout << "-s <minutes> Simulation duration" << std::endl;
	std::cout << "-m <minutes> Package manufacturing time" << std::endl;
	std::cout << "--df <ml> Diesel fuel capacity" << std::endl;
	std::cout << "--ef <Wh> Electric battery capacity" << std::endl;
	std::cout << "--dc <ml/km> Diesel fuel consumption" << std::endl;
	std::cout << "--ec <Wh/km> Electric battery consumption" << std::endl;
}

std::string ArgumentParser::getArgument()
{
	if (optarg == NULL && optind < this->argc && this->argv[optind][0] != '-')
	{
		return this->argv[optind++];
	}
	else
	{
		return optarg;
	}
}

int ArgumentParser::getNumericArgument()
{
	try
	{
		return std::stoi(this->getArgument());
	}
	catch (const std::invalid_argument &e)
	{
		std::cerr << "Invalid argument type" << std::endl;
		exit(INVALID_ARUMENT_ERROR);
	}
}

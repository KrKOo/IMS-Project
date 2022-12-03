#include <iostream>
#include <getopt.h>

#include "ArgumentParser.hpp"

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
	const char *shortOptions = "n:f:c:t:s:m";
	struct option longOptions[] = {
		{"help", no_argument, 0, 'h'},
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
		case 'n':
			arguments.truckCount = getNumericArgument();
			break;
		case 'f':
			arguments.fuelTankCapacity = getNumericArgument();
			break;
		case 'c':
			arguments.fuelConsumption = getNumericArgument();
			break;
		case 't':
		{
			std::string value = getArgument();
			if (value == "e")
			{
				arguments.isElectric = true;
			}
			else if (value == "d")
			{
				arguments.isElectric = false;
			}
			else
			{
				std::cerr << "Invalid argument for -t: " << value << std::endl;
				printHelp();
				exit(INVALID_ARUMENT_ERROR);
			}
			break;
		}
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
	std::cout << "-n - Truck count" << std::endl;
	std::cout << "-f - Fuel capacity" << std::endl;
	std::cout << "-c - Fuel consumption /100km" << std::endl;
	std::cout << "-t - Truck type [e|d]" << std::endl;
	std::cout << "-s - Simulation duration" << std::endl;
	std::cout << "-m - Package manufacturing time" << std::endl;
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

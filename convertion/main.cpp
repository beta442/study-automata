#include "include/pch.h"

#include "include/ArgParse/ParseArgs.h"
#include "include/CSV/csv.hpp"

#include "include/Automata/MooreTable.hpp"
#include "include/Automata/MealyTable.hpp"
#include "include/Automata/State.hpp"

int main(int argc, char* argv[])
{
	auto program = ParseArgs(argc, argv);

	auto& inputFileName = program.get(INPUT_FILE_PAR);
	auto& outputFileName = program.get(OUTPUT_FILE_PAR);

	auto& mode = program.get(MODE_PAR);

	try
	{
		std::ofstream oFS{ outputFileName };

		if (mode == MEALY_TO_MOORE)
		{
			auto mealyTable = MealyTable{ inputFileName };
			auto mooreTable = MooreTable{ mealyTable };
			oFS << mooreTable;
		}
		if (mode == MOORE_TO_MEALY)
		{
		}
	}
	catch (const std::exception& e)
	{
		std::cout << e.what() << std::endl;
	}

	return 0;
}

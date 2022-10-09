#include "include/pch.h"

#include "include/ArgParse/ParseArgs.h"
#include "include/CSV/csv.hpp"

#include "include/Automata/MealyTableReader.hpp"
#include "include/Automata/MooreTableReader.hpp"

#include "include/Automata/MealyMooreTable.hpp"

int main(int argc, char* argv[])
{
	auto program = ParseArgs(argc, argv);

	auto& inputFileName = program.get(INPUT_FILE_PAR);
	auto& outputFileName = program.get(OUTPUT_FILE_PAR);

	auto& mode = program.get(MODE_PAR);

	try
	{
		auto reader = csv::CSVReader(inputFileName);
		std::ofstream oFS{ outputFileName };

		if (mode == MEALY_TO_MOORE)
		{
			auto mealyTableReader = MealyTableReader{ reader };
			auto mooreTable = MooreTable{
				MealyTable{
					mealyTableReader.GetMealyStates(),
					mealyTableReader.GetStates(),
					mealyTableReader.GetTransitions() }
			};
			oFS << mooreTable;
		}
		if (mode == MOORE_TO_MEALY)
		{
			auto mooreTableReader = MooreTableReader{ reader };
			auto mealyTable = MealyTable{
				MooreTable{
					mooreTableReader.GetSignals(),
					mooreTableReader.GetStates(),
					mooreTableReader.GetTransitions(),
					mooreTableReader.GetMooreTable() }
			};
			oFS << mealyTable;
		}
	}
	catch (const std::exception& e)
	{
		std::cout << e.what() << std::endl;
		std::exit(1);
	}

	return 0;
}

#include "include/pch.h"

#include "include/ArgParse/ParseArgs.h"
#include "include/CSV/csv.hpp"

#include "include/Automata/MealyTable.hpp"
#include "include/Automata/State.hpp"

int main(int argc, char* argv[])
{
	auto program = ParseArgs(argc, argv);

	auto& inputFileName = program.get(INPUT_FILE_PAR);
	auto& outputFileName = program.get(OUTPUT_FILE_PAR);

	auto& mode = program.get(MODE_PAR);

	std::cout << "Input: " << inputFileName
			  << "\nOutput: " << outputFileName
			  << "\nMode: " << mode << std::endl;

	try
	{
		auto mTable = MealyTable{ inputFileName };
		std::cout << mTable;
	}
	catch (const std::exception& e)
	{
		std::cout << e.what() << std::endl;
	}
	/*using namespace csv;

	CSVReader reader(inputFileName);
	for (auto& cName : reader.get_col_names())
	{
		std::cout << cName << std::endl;
	}

	for (auto& row : reader)
	{
		for (auto& field : row)
		{
			std::cout << MealyState{ field.get() } << ";";
		}
		std::cout << std::endl;
	}*/

	return 0;
}

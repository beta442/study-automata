#include "../../include/ArgParse/ParseArgs.h"

argparse::ArgumentParser ParseArgs(int argc, char* argv[])
{
	argparse::ArgumentParser program("transform", "0.0.1");

	program.add_description("Transform Mealy to Moore or Moore to Mealy automatons from " + std::string(INPUT_FILE_PAR) + " to " + std::string(OUTPUT_FILE_PAR));

	program.add_argument(MODE_PAR)
		.help("specify transform's mode {" + std::string(MEALY_TO_MOORE) + '|' + std::string(MOORE_TO_MEALY) + '}')
		.nargs(1)
		.required();

	program.add_argument(INPUT_FILE_PAR)
		.help("source file with source automaton")
		.nargs(1)
		.required();

	program.add_argument(OUTPUT_FILE_PAR)
		.help("destination file")
		.nargs(1)
		.required();

	try
	{
		program.parse_args(argc, argv);
		if (auto mode = program.get(MODE_PAR); mode != MEALY_TO_MOORE && mode != MOORE_TO_MEALY)
		{
			throw std::invalid_argument("Wrong " + std::string(MODE_PAR) + " provided");
		}
	}
	catch (const std::exception& err)
	{
		std::cerr << err.what() << std::endl;
		std::cerr << program;
		std::exit(1);
	}

	return program;
}

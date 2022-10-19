#include "../../include/ArgParse/ParseArgs.h"

argparse::ArgumentParser ParseArgs(int argc, char* argv[])
{
	argparse::ArgumentParser program("automata", "0.0.2");

	program.add_description("Performs operations on Mealy|Moore automatons.");


	program.add_argument(MODE_PAR)
		.help("specify transform's mode {" +
			std::string(MEALY_TO_MOORE) + '|' +
			std::string(MOORE_TO_MEALY) + '|' +
			std::string(MEALY_MIN) + '|' +
			std::string(MOORE_MIN) + '}')
		.action([](const auto& s) noexcept {
			return StringToProgramMode(s);
		})
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
		if (auto mode = program.get<ProgramMode>(MODE_PAR);
			mode == ProgramMode::UNKNOWN)
		{
			throw std::invalid_argument("Wrong " + std::string(MODE_PAR) + " provided. See help");
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

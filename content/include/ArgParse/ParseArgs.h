#ifndef ARGPARSE_PARSE_ARGS_H_
#define ARGPARSE_PARSE_ARGS_H_

#include "argparse.hpp"

constexpr auto MODE_PAR = "<mode>";
constexpr auto MEALY_MIN = "mealy";
constexpr auto MEALY_TO_MOORE = "mealy-to-moore";
constexpr auto MOORE_MIN = "moore";
constexpr auto MOORE_TO_MEALY = "moore-to-mealy";

enum class ProgramMode
{
	MEALY_MIN = 0,
	MEALY_TO_MOORE,
	MOORE_MIN,
	MOORE_TO_MEALY,
	UNKNOWN,
};

inline ProgramMode StringToProgramMode(const std::string& str)
{
	if (str == MEALY_MIN)
	{
		return ProgramMode::MEALY_MIN;
	}
	if (str == MEALY_TO_MOORE)
	{
		return ProgramMode::MEALY_TO_MOORE;
	}
	if (str == MOORE_MIN)
	{
		return ProgramMode::MOORE_MIN;
	}
	if (str == MOORE_TO_MEALY)
	{
		return ProgramMode::MOORE_TO_MEALY;
	}
	return ProgramMode::UNKNOWN;
}

constexpr auto INPUT_FILE_PAR = "<input-file>";
constexpr auto OUTPUT_FILE_PAR = "<output-file>";

argparse::ArgumentParser ParseArgs(int argc, char* argv[]);

#endif // !PARSE_ARGS_H

#ifndef ARGPARSE_PARSE_ARGS_H_
#define ARGPARSE_PARSE_ARGS_H_

#include "argparse.hpp"

constexpr auto MODE_PAR = "<mode>";
constexpr auto MEALY_TO_MOORE = "mealy-to-moore";
constexpr auto MOORE_TO_MEALY = "moore-to-mealy";

constexpr auto INPUT_FILE_PAR = "<input-file>";
constexpr auto OUTPUT_FILE_PAR = "<output-file>";

argparse::ArgumentParser ParseArgs(int argc, char* argv[]);

#endif // !PARSE_ARGS_H

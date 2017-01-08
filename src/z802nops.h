/**
 * @author Romain Giot <giot.romain@gmail.com>
 * @licence GPL
 * @date 01/2017
 */



#include <string>
#include <regex>
#include <vector>
#include <iostream>
#include <algorithm>
#include <fstream>
namespace z80tonops {


/**
 * Extract the instruction from the line of interest
 */
const std::string extract_instruction_from_line(const std::string & line);

/**
 * Compute the duration of the instruction.
 * Assumes ONLY the instruction is present on the string (no label, comment, start/end whitesapces)
 */
size_t duration(const std::string & instruction);

/**
 * 1. read a stream of z80 code
 * 2. compute the timing for each instruction
 * 3. output the stream of code BUT add a comment with the number of nops after each instruction
 * 4. add a final comment with the global timing value
 */
void treat_stream(std::istream & istream, std::ostream & ostream);
}


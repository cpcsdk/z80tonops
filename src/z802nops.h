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


struct Timing {
    size_t main, optionnal;

    Timing(const size_t main): main{main}, optionnal{0} {
    }

    Timing(const size_t main, const size_t optionnal): main{main}, optionnal{optionnal} {
    }


    Timing() = delete;
    Timing(const Timing &) = default;
    ~Timing() = default;

    /**
     * By default, only the main duration is returned.
     * Main depends on the type of instruction ...
     */
    operator size_t () const {
        return main;
    }


    bool hasSimpleTiming() const {
        return optionnal == 0;
    }

};



/**
 * Extract the instruction from the line of interest
 */
const std::string extract_instruction_from_line(const std::string & line);

/**
 * Compute the duration of the instruction.
 * Assumes ONLY the instruction is present on the string (no label, comment, start/end whitesapces)
 */
Timing duration(const std::string & instruction);

/**
 * 1. read a stream of z80 code
 * 2. compute the timing for each instruction
 * 3. output the stream of code BUT add a comment with the number of nops after each instruction
 * 4. add a final comment with the global timing value
 */
void treat_stream(std::istream & istream, std::ostream & ostream);
}


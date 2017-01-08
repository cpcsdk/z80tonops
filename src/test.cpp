/**
 * @author Romain Giot <giot.romain@gmail.com>
 * @licence GPL
 * @date 01/2017
 */


#include <cassert>
#include "z802nops.h"
#include <string>
#include <iostream>

void T(const std::string & opcode, size_t nops) {
    std::cerr << "Opcode: " << opcode << " Duration: " << z80tonops::duration(opcode) << " Expected: " <<nops << std::endl;
    assert(z80tonops::duration(opcode) == nops);
}


int main(int argc, char** argv) {
    T(" LD A, 0", 2);
    T(" LD A, 10 + (5+2)", 2);
    T(" LD A, (0)", 4);
    T(" LD (10), A", 4);
    T(" LD HL, (50)", 5);
    T(" LD BC, (50)", 6);
    T(" LD SP, (50)", 6);
}

